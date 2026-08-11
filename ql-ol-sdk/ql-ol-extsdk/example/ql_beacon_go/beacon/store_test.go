package beacon

import (
	"os"
	"path/filepath"
	"testing"
)

func TestPendingStoreSaveLoadDeleteRoundTrip(t *testing.T) {
	dir := t.TempDir()
	store, err := OpenPendingStore(dir)
	if err != nil {
		t.Fatalf("OpenPendingStore: %v", err)
	}

	id1, err := store.Save([]byte("beacon-one"))
	if err != nil {
		t.Fatalf("Save: %v", err)
	}
	id2, err := store.Save([]byte("beacon-two"))
	if err != nil {
		t.Fatalf("Save: %v", err)
	}
	if id1 == id2 {
		t.Fatalf("Save returned the same id twice: %d", id1)
	}

	records, err := store.LoadAll()
	if err != nil {
		t.Fatalf("LoadAll: %v", err)
	}
	if len(records) != 2 {
		t.Fatalf("got %d records, want 2", len(records))
	}
	if records[0].ID != id1 || string(records[0].Payload) != "beacon-one" {
		t.Errorf("record[0] = %+v, want id %d payload %q", records[0], id1, "beacon-one")
	}
	if records[1].ID != id2 || string(records[1].Payload) != "beacon-two" {
		t.Errorf("record[1] = %+v, want id %d payload %q", records[1], id2, "beacon-two")
	}

	if err := store.Delete(id1); err != nil {
		t.Fatalf("Delete: %v", err)
	}
	records, err = store.LoadAll()
	if err != nil {
		t.Fatalf("LoadAll after delete: %v", err)
	}
	if len(records) != 1 || records[0].ID != id2 {
		t.Fatalf("after deleting id1, got %+v, want only id %d", records, id2)
	}

	// Deleting something already gone is not an error.
	if err := store.Delete(id1); err != nil {
		t.Errorf("Delete of already-gone id: %v", err)
	}
}

func TestPendingStoreSurvivesReopen(t *testing.T) {
	dir := t.TempDir()

	store, err := OpenPendingStore(dir)
	if err != nil {
		t.Fatalf("OpenPendingStore: %v", err)
	}
	id, err := store.Save([]byte("outstanding"))
	if err != nil {
		t.Fatalf("Save: %v", err)
	}

	// Simulate a crash/reboot: nothing more happens on the original store,
	// a brand new one opens the same directory (mirrors Dial being called
	// again after a process restart or a reconnect).
	reopened, err := OpenPendingStore(dir)
	if err != nil {
		t.Fatalf("re-OpenPendingStore: %v", err)
	}
	records, err := reopened.LoadAll()
	if err != nil {
		t.Fatalf("LoadAll: %v", err)
	}
	if len(records) != 1 || records[0].ID != id || string(records[0].Payload) != "outstanding" {
		t.Fatalf("got %+v, want the one unacked record to survive reopen", records)
	}

	// ids assigned by the reopened store must not collide with the
	// already-committed one.
	newID, err := reopened.Save([]byte("second"))
	if err != nil {
		t.Fatalf("Save after reopen: %v", err)
	}
	if newID == id {
		t.Errorf("reopened store reused id %d", id)
	}
}

func TestPendingStoreSkipsCorruptRecordButLoadsRest(t *testing.T) {
	dir := t.TempDir()
	store, err := OpenPendingStore(dir)
	if err != nil {
		t.Fatalf("OpenPendingStore: %v", err)
	}

	goodID, err := store.Save([]byte("good"))
	if err != nil {
		t.Fatalf("Save: %v", err)
	}

	// Hand-craft a corrupt committed record (as if a torn write slipped
	// through despite the fsync-before-rename ordering) under a different
	// id than the good one, to prove one bad record doesn't block the rest.
	corruptPath := filepath.Join(dir, "00000000000000000001.bcn")
	if err := os.WriteFile(corruptPath, []byte("not a valid record"), 0600); err != nil {
		t.Fatalf("writing corrupt record: %v", err)
	}

	records, err := store.LoadAll()
	if err != nil {
		t.Fatalf("LoadAll: %v", err)
	}
	if len(records) != 1 || records[0].ID != goodID {
		t.Fatalf("got %+v, want only the good record to survive a corrupt sibling", records)
	}
}

func TestOpenPendingStoreCleansUpStrayTempFiles(t *testing.T) {
	dir := t.TempDir()
	tmpPath := filepath.Join(dir, "00000000000000000005.bcn.tmp")
	if err := os.WriteFile(tmpPath, []byte("never committed"), 0600); err != nil {
		t.Fatalf("writing stray temp file: %v", err)
	}

	store, err := OpenPendingStore(dir)
	if err != nil {
		t.Fatalf("OpenPendingStore: %v", err)
	}
	if _, err := os.Stat(tmpPath); !os.IsNotExist(err) {
		t.Errorf("stray temp file should have been removed, stat err = %v", err)
	}

	// A never-committed write was never durable, so the next id should not
	// be forced past it.
	id, err := store.Save([]byte("first real record"))
	if err != nil {
		t.Fatalf("Save: %v", err)
	}
	if id != 0 {
		t.Errorf("first id after cleaning a stray temp = %d, want 0", id)
	}
}

func TestPendingStoreEvictsOldestWhenFull(t *testing.T) {
	original := maxQueuedRecords
	maxQueuedRecords = 5 // shrink the cap so the test doesn't need thousands of records
	defer func() { maxQueuedRecords = original }()

	dir := t.TempDir()
	store, err := OpenPendingStore(dir)
	if err != nil {
		t.Fatalf("OpenPendingStore: %v", err)
	}

	var firstID uint64
	for i := 0; i < maxQueuedRecords; i++ {
		id, err := store.Save([]byte("filler"))
		if err != nil {
			t.Fatalf("Save #%d: %v", i, err)
		}
		if i == 0 {
			firstID = id
		}
	}

	if _, err := store.Save([]byte("one too many")); err != nil {
		t.Fatalf("Save over capacity: %v", err)
	}

	records, err := store.LoadAll()
	if err != nil {
		t.Fatalf("LoadAll: %v", err)
	}
	if len(records) != maxQueuedRecords {
		t.Fatalf("got %d records, want capped at %d", len(records), maxQueuedRecords)
	}
	for _, r := range records {
		if r.ID == firstID {
			t.Errorf("oldest record (id %d) should have been evicted to make room", firstID)
		}
	}
}
