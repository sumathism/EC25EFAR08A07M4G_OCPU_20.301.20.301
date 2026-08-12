package beacon

import (
	"fmt"
	"log"
	"net"
	"sync"
	"time"
)

// loginSeqID is the fixed sequence ID the firmware uses for the one-time
// login beacon that bootstraps a secure session.
const loginSeqID = 0xFFFF

// Session is one TCP connection to the beacon server: it performs the
// ECDH+login handshake on Dial, then wraps and transmits inner beacons
// (already-finished frames, e.g. a type-100 device-info beacon) as
// ChaCha20-Poly1305-encrypted type-102 secure beacons. It also reads the
// connection in the background, surfacing the server's BEACON_ACKS_6
// responses (sequence IDs it has acknowledged) via Acks(), and resends any
// beacon (login or application) that goes unacked for too long - see
// pending.go. All of this is generic over whatever beacon content is passed
// to SendBeacon, so future beacon types get retry-on-timeout for free.
//
// Application beacons passed to SendBeacon are durably queued (see
// PendingStore, store.go) before any network write is attempted, and only
// removed once the server acks them - so unlike the connection itself,
// that queue must outlive any single Session. Dial reopens the same
// on-disk queue directory every time (first boot and every later reconnect
// alike) and folds whatever's still outstanding into the new Session's
// pending list, which is what makes a beacon survive a reboot, a watchdog
// reset, or Dial being called again after a dropped connection.
type Session struct {
	conn    net.Conn
	shared  [32]byte
	seq     uint16
	acks    chan []uint16
	retries chan uint16
	closed  chan struct{}

	store *PendingStore

	mu      sync.Mutex
	pending []*pendingBeacon
}

// Dial connects to addr (host:port), derives the shared secret against the
// compiled-in server public key, opens (or reopens) the durable beacon
// queue at queueDir, and sends the login beacon.
//
// hostID is the device's own hex-encoded DEVICEID string - the login
// module's payload is HostIDLen(1B) + HostID(len bytes) + PublicKey(32B),
// not just the bare public key (confirmed against lafm_beacon_create_102_add_pubkey_1
// in the firmware, which fetches and prepends this internally).
func Dial(addr string, identity *Identity, hostID string, timeout time.Duration, queueDir string) (*Session, error) {
	store, err := OpenPendingStore(queueDir)
	if err != nil {
		return nil, fmt.Errorf("opening durable beacon queue: %w", err)
	}

	conn, err := net.DialTimeout("tcp", addr, timeout)
	if err != nil {
		return nil, err
	}

	serverPublic, err := ServerPublicKey()
	if err != nil {
		conn.Close()
		return nil, err
	}
	shared, err := identity.SharedSecret(serverPublic)
	if err != nil {
		conn.Close()
		return nil, err
	}

	s := &Session{
		conn:    conn,
		shared:  shared,
		acks:    make(chan []uint16, 16),
		retries: make(chan uint16, 16),
		closed:  make(chan struct{}),
		store:   store,
	}

	// Fold in anything still outstanding from before this Dial - a beacon
	// durably queued but never acked, whether the process crashed, the
	// device rebooted, or this is simply a reconnect after the previous
	// Session died. sentAt is left at its zero value, so retryStale's very
	// first tick (within retryCheckEvery) treats each as already overdue
	// and gives it its first real send attempt on this connection - no
	// separate resume codepath needed.
	recovered, err := store.LoadAll()
	if err != nil {
		log.Printf("beacon: loading durable queue %s (continuing without recovering it): %v", queueDir, err)
	}
	for _, rec := range recovered {
		s.pending = append(s.pending, &pendingBeacon{id: rec.ID, inner: rec.Payload, durable: true})
	}

	pub := identity.PublicKey()
	loginPayload := make([]byte, 0, 1+len(hostID)+32)
	loginPayload = append(loginPayload, byte(len(hostID)))
	loginPayload = append(loginPayload, hostID...)
	loginPayload = append(loginPayload, pub[:]...)

	login := NewBeacon(TypeSecure, loginSeqID)
	login.AddModule(ModuleLoginPubKey, loginPayload)
	loginFrame := login.Finish()
	if _, err := conn.Write(loginFrame); err != nil {
		conn.Close()
		return nil, err
	}
	s.trackPending(loginSeqID, loginFrame)

	go s.readLoop()
	go s.retryLoop()
	return s, nil
}

// Acks delivers batches of sequence IDs the server has acknowledged
// (BEACON_ACKS_6, arriving as a plaintext module inside a type-102 frame).
// It's closed when the connection's read side ends.
func (s *Session) Acks() <-chan []uint16 {
	return s.acks
}

// Retries delivers the sequence ID of every beacon resent because no ack
// arrived within retryTimeout. Closed when the session is closed.
func (s *Session) Retries() <-chan uint16 {
	return s.retries
}

// readLoop parses inbound frames for as long as the connection is alive and
// forwards any ack module payloads. It exits (closing acks) once the
// connection errors or is closed - the caller notices the session is dead
// via a subsequent SendBeacon failure and reconnects, which starts a fresh
// readLoop for the new Session.
func (s *Session) readLoop() {
	defer close(s.acks)
	reader := NewFrameReader(s.conn)
	for {
		frame, err := reader.ReadFrame()
		if err != nil {
			return
		}
		if frame.BeaconType != TypeSecure {
			continue
		}
		payload, found := frame.Module(ModuleBeaconAcks)
		if !found {
			continue
		}
		acks := ParseBeaconAcks(payload)
		if len(acks) == 0 {
			continue
		}
		s.clearPending(acks)
		select {
		case s.acks <- acks:
		default: // don't block the reader if nobody's draining acks fast enough
		}
	}
}

// SendBeacon durably persists a fully-built inner beacon frame (its own
// type/seq/CRC already set, e.g. from NewBeacon(TypeFromDevice, ...).Finish())
// to the beacon queue on disk, then wraps it in a type-102 secure beacon and
// attempts to transmit it - this applies to any beacon content passed in,
// so it's shared by every beacon type without changes.
//
// The durable write happens first and is complete (fsync'd, committed under
// its final filename) before any network write is attempted, so even if
// the write to the network fails right after, or the process crashes, or
// the device reboots, the beacon isn't lost - it's already safe on disk and
// will go out (re-wrapped fresh, see transmit) the next time this session
// or a later reconnect's Dial recovers it.
func (s *Session) SendBeacon(inner []byte) error {
	id, err := s.store.Save(inner)
	if err != nil {
		return fmt.Errorf("persisting beacon to durable queue: %w", err)
	}
	log.Printf("beacon: queued record %d (%d bytes) to durable store", id, len(inner))
	pb := &pendingBeacon{id: id, inner: inner, durable: true}
	s.mu.Lock()
	s.pending = append(s.pending, pb)
	s.mu.Unlock()
	return s.transmit(pb)
}

// transmit wraps pb.inner in a fresh type-102 secure beacon - a new outer
// sequence number and a new random nonce - and writes it. Every (re)send
// attempt calls this rather than replaying a previously built outer frame,
// because the outer seq is scoped to one TCP session and resets to zero on
// every reconnect (see Dial): a frame built under a stale seq could collide
// with, or simply be meaningless relative to, the new session's numbering.
// Persisting and replaying the inner payload instead - re-wrapping it fresh
// every time - mirrors the firmware's own split between its durable NOR
// flash queue (raw beacon payload) and the secure/session wrap applied only
// at actual transmit time.
func (s *Session) transmit(pb *pendingBeacon) error {
	s.mu.Lock()
	s.seq++
	seq := s.seq
	s.mu.Unlock()

	cipherPayload, err := buildChaChaCipherModule(s.shared, pb.inner)
	if err != nil {
		return err
	}
	outer := NewBeacon(TypeSecure, seq)
	outer.AddModule(ModuleChaChaCipher, cipherPayload)
	frame := outer.Finish()
	if _, err := s.conn.Write(frame); err != nil {
		return err
	}

	s.mu.Lock()
	pb.seq = seq
	pb.sentAt = time.Now()
	s.mu.Unlock()
	log.Printf("beacon: sent durable record %d as seq %d", pb.id, seq)
	return nil
}

// LastSeq returns the outer (type-102) sequence ID most recently sent -
// this is what the server's acks reference, for correlating log lines.
func (s *Session) LastSeq() uint16 {
	return s.seq
}

// Close stops the retry loop and closes the underlying connection (which in
// turn ends readLoop).
func (s *Session) Close() error {
	close(s.closed)
	return s.conn.Close()
}
