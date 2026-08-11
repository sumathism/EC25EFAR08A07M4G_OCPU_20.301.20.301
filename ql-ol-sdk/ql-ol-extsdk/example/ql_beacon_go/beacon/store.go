package beacon

import (
	"errors"
	"fmt"
	"log"
	"os"
	"path/filepath"
	"sort"
	"strconv"
	"strings"
	"sync"
)

// PendingStore durably queues not-yet-acknowledged inner beacon payloads on
// disk - in practice /mnt, the UBIFS filesystem mounted on the Winbond
// W25N01GW SPI-NAND (see ql-ol-rootfs/etc/init.d/find_partitions.sh and the
// mt29f_spinand driver) - so a beacon survives a process crash, a watchdog
// reset, or a reconnect, exactly like the firmware's lafm_nor_nq durable
// queue used for its logged/status/device-info beacons (as opposed to the
// RAM-only "realtime/live" path the firmware itself accepts losing on
// reset). Unlike lafm_nor_nq, there's no need to hand-roll a circular log
// with version-tagged blocks here - UBIFS already gives crash-consistent
// file writes and wear leveling, so this only needs plain file I/O plus an
// fsync-before-rename to make the "written before acted upon" ordering
// explicit.
//
// One inner beacon = one file, named by a monotonically increasing id
// (zero-padded so filename sort order == arrival order). A record is only
// visible under its final name once fully written and fsync'd - see Save.
type PendingStore struct {
	dir   string
	mu    sync.Mutex
	next  uint64
	count int // mirrors the number of committed *.bcn files, kept in memory so Save doesn't need a directory scan on every call
}

// durableRecord is one beacon recovered from disk by LoadAll.
type durableRecord struct {
	ID      uint64
	Payload []byte
}

const (
	recordSuffix = ".bcn"
	tmpSuffix    = ".tmp"
	recordMagic  = "LBCN"
	// magic(4B) + payloadLen(4B).
	recordHeaderSize = 4 + 4
)

// maxQueuedRecords bounds worst-case disk usage during a prolonged outage.
// /mnt is a whole 128MB NAND with no smaller partition split observed for
// it, so this is generous - it exists only as a backstop, not a normal
// operating limit. Hitting it always logs a warning rather than silently
// dropping data, unlike the old RAM-only queue's silent 32-slot eviction.
// A var (not a const) only so tests can shrink it instead of writing tens
// of thousands of records to exercise the eviction path.
var maxQueuedRecords = 10000

// OpenPendingStore opens (creating if needed) the durable queue directory
// dir, discards any stray *.tmp files left behind by a write that never
// completed (a crash between OpenFile and Rename - the record was never
// committed under its final name, so it was never "durable"), and seeds the
// next id past whatever's already on disk so ids stay monotonic across
// restarts.
func OpenPendingStore(dir string) (*PendingStore, error) {
	if err := os.MkdirAll(dir, 0755); err != nil {
		return nil, fmt.Errorf("creating durable queue dir %s: %w", dir, err)
	}
	s := &PendingStore{dir: dir}
	if err := s.cleanupTemps(); err != nil {
		log.Printf("beacon: cleaning up stray temp files in %s: %v", dir, err)
	}
	names, err := s.listRecordFiles()
	if err != nil {
		return nil, fmt.Errorf("scanning durable queue dir %s: %w", dir, err)
	}
	var next uint64
	for _, name := range names {
		if id, ok := idFromRecordFileName(name); ok && id+1 > next {
			next = id + 1
		}
	}
	s.next = next
	s.count = len(names)
	return s, nil
}

// Save durably writes payload as a new record and returns its id. The
// write is fsync'd and the file committed (atomic rename into place)
// before Save returns, so by the time the caller goes on to transmit the
// beacon, it's already safe on disk - a crash or reboot immediately after
// can't lose it.
func (s *PendingStore) Save(payload []byte) (uint64, error) {
	s.mu.Lock()
	defer s.mu.Unlock()

	if s.count >= maxQueuedRecords {
		if err := s.evictOldest(); err != nil {
			log.Printf("beacon: durable queue cap check failed (continuing anyway): %v", err)
		} else {
			s.count--
		}
	}

	id := s.next
	if err := s.writeRecord(id, payload); err != nil {
		return 0, err
	}
	s.next++
	s.count++
	return id, nil
}

// Delete removes a record once its beacon has been acknowledged - the
// literal "pop from the durable queue" step. Deleting an id that's already
// gone (e.g. evicted under the cap) is not an error.
func (s *PendingStore) Delete(id uint64) error {
	s.mu.Lock()
	defer s.mu.Unlock()

	err := os.Remove(filepath.Join(s.dir, recordFileName(id)))
	if err != nil {
		if os.IsNotExist(err) {
			return nil
		}
		return err
	}
	s.count--
	return nil
}

// LoadAll returns every record still on disk, oldest first - the beacons
// that were durably queued but never got an ack, whether because the
// process crashed, the device rebooted, or the connection dropped before
// the server's ack arrived. A corrupt record (e.g. a torn write that still
// slipped through, in principle, despite the fsync-before-rename ordering)
// is logged and skipped rather than blocking recovery of the rest.
func (s *PendingStore) LoadAll() ([]durableRecord, error) {
	s.mu.Lock()
	defer s.mu.Unlock()

	names, err := s.listRecordFiles()
	if err != nil {
		return nil, fmt.Errorf("scanning durable queue dir %s: %w", s.dir, err)
	}
	records := make([]durableRecord, 0, len(names))
	for _, name := range names {
		id, ok := idFromRecordFileName(name)
		if !ok {
			continue
		}
		payload, err := s.readRecord(name)
		if err != nil {
			log.Printf("beacon: skipping corrupt durable record %s: %v", name, err)
			continue
		}
		records = append(records, durableRecord{ID: id, Payload: payload})
	}
	return records, nil
}

// evictOldest drops the single oldest record, with a warning. Called with
// s.mu already held, only once Save has determined the queue is at
// maxQueuedRecords - the directory scan this needs to find "oldest" is
// then a one-off, not something every Save pays for.
func (s *PendingStore) evictOldest() error {
	names, err := s.listRecordFiles()
	if err != nil {
		return err
	}
	if len(names) == 0 {
		return nil
	}
	oldest := names[0]
	log.Printf("beacon: durable queue at capacity (%d records) - dropping oldest queued beacon (%s) to make room", maxQueuedRecords, oldest)
	return os.Remove(filepath.Join(s.dir, oldest))
}

// writeRecord builds the on-disk record layout - magic(4B) + payloadLen(4B
// LE) + payload + crc16(2B) + crc16(2B duplicate copy) - and commits it via
// write-to-temp, fsync, atomic rename, the same torn-write-tolerant pattern
// crypto.go already uses for the identity key file.
func (s *PendingStore) writeRecord(id uint64, payload []byte) error {
	name := recordFileName(id)
	tmpPath := filepath.Join(s.dir, name+tmpSuffix)
	finalPath := filepath.Join(s.dir, name)

	buf := make([]byte, 0, recordHeaderSize+len(payload)+4)
	buf = append(buf, recordMagic...)
	lenBytes := make([]byte, 4)
	putU32LE(lenBytes, uint32(len(payload)))
	buf = append(buf, lenBytes...)
	buf = append(buf, payload...)
	crc := crc16(buf)
	crcBytes := make([]byte, 2)
	putU16LE(crcBytes, crc)
	buf = append(buf, crcBytes...)
	buf = append(buf, crcBytes...)

	f, err := os.OpenFile(tmpPath, os.O_WRONLY|os.O_CREATE|os.O_TRUNC, 0600)
	if err != nil {
		return fmt.Errorf("creating %s: %w", tmpPath, err)
	}
	if _, err := f.Write(buf); err != nil {
		f.Close()
		os.Remove(tmpPath)
		return fmt.Errorf("writing %s: %w", tmpPath, err)
	}
	if err := f.Sync(); err != nil {
		f.Close()
		os.Remove(tmpPath)
		return fmt.Errorf("syncing %s: %w", tmpPath, err)
	}
	if err := f.Close(); err != nil {
		os.Remove(tmpPath)
		return fmt.Errorf("closing %s: %w", tmpPath, err)
	}
	if err := os.Rename(tmpPath, finalPath); err != nil {
		return fmt.Errorf("committing %s: %w", finalPath, err)
	}
	return nil
}

func (s *PendingStore) readRecord(name string) ([]byte, error) {
	data, err := os.ReadFile(filepath.Join(s.dir, name))
	if err != nil {
		return nil, err
	}
	if len(data) < recordHeaderSize+2+2 {
		return nil, errors.New("record too short")
	}
	if string(data[0:4]) != recordMagic {
		return nil, errors.New("bad magic")
	}
	payloadLen := int(getU32LE(data[4:8]))
	end := recordHeaderSize + payloadLen
	if end+4 != len(data) {
		return nil, fmt.Errorf("length mismatch: header says %d byte payload, file is %d bytes", payloadLen, len(data))
	}
	covered := data[:end]
	crc1 := getU16LE(data[end : end+2])
	crc2 := getU16LE(data[end+2 : end+4])
	if computed := crc16(covered); computed != crc1 && computed != crc2 {
		return nil, errors.New("crc mismatch")
	}
	payload := make([]byte, payloadLen)
	copy(payload, data[recordHeaderSize:end])
	return payload, nil
}

// cleanupTemps removes any *.tmp files left behind by a write that never
// completed. Best-effort: a leftover temp file that can't be removed isn't
// worth failing startup over.
func (s *PendingStore) cleanupTemps() error {
	entries, err := os.ReadDir(s.dir)
	if err != nil {
		return err
	}
	for _, e := range entries {
		if !e.IsDir() && strings.HasSuffix(e.Name(), tmpSuffix) {
			os.Remove(filepath.Join(s.dir, e.Name()))
		}
	}
	return nil
}

// listRecordFiles returns committed (non-temp) record filenames, sorted -
// zero-padded ids make lexicographic order equal to arrival order.
func (s *PendingStore) listRecordFiles() ([]string, error) {
	entries, err := os.ReadDir(s.dir)
	if err != nil {
		return nil, err
	}
	var names []string
	for _, e := range entries {
		if !e.IsDir() && strings.HasSuffix(e.Name(), recordSuffix) {
			names = append(names, e.Name())
		}
	}
	sort.Strings(names)
	return names, nil
}

func recordFileName(id uint64) string {
	return fmt.Sprintf("%020d%s", id, recordSuffix)
}

func idFromRecordFileName(name string) (uint64, bool) {
	id, err := strconv.ParseUint(strings.TrimSuffix(name, recordSuffix), 10, 64)
	if err != nil {
		return 0, false
	}
	return id, true
}

func putU32LE(dst []byte, v uint32) {
	dst[0] = byte(v)
	dst[1] = byte(v >> 8)
	dst[2] = byte(v >> 16)
	dst[3] = byte(v >> 24)
}

func getU32LE(b []byte) uint32 {
	return uint32(b[0]) | uint32(b[1])<<8 | uint32(b[2])<<16 | uint32(b[3])<<24
}
