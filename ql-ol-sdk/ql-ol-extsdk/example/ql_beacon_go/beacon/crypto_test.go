package beacon

import (
	"os"
	"path/filepath"
	"testing"
)

func TestIdentityPersistsAcrossReload(t *testing.T) {
	path := filepath.Join(t.TempDir(), "devicekey")

	id1, err := LoadOrCreateIdentity(path)
	if err != nil {
		t.Fatalf("first LoadOrCreateIdentity: %v", err)
	}
	pub1 := id1.PublicKey()

	// Simulate a process restart: load again from the same path.
	id2, err := LoadOrCreateIdentity(path)
	if err != nil {
		t.Fatalf("second LoadOrCreateIdentity: %v", err)
	}
	pub2 := id2.PublicKey()

	if pub1 != pub2 {
		t.Fatalf("public key changed across reload: %x != %x", pub1, pub2)
	}

	data, err := os.ReadFile(path)
	if err != nil {
		t.Fatalf("reading persisted file: %v", err)
	}
	if len(data) != keyRecordSize {
		t.Fatalf("persisted record size = %d, want %d", len(data), keyRecordSize)
	}
	if string(data[0:4]) != keyMagic {
		t.Fatalf("persisted record magic = %q, want %q", data[0:4], keyMagic)
	}
	// The stored public key should match what PublicKey() returns, not be re-derived.
	var storedPub [32]byte
	copy(storedPub[:], data[36:68])
	if storedPub != pub1 {
		t.Fatalf("stored public key in file doesn't match Identity.PublicKey()")
	}
}

func TestIdentityRegeneratesOnCorruptRecord(t *testing.T) {
	path := filepath.Join(t.TempDir(), "devicekey")

	id1, err := LoadOrCreateIdentity(path)
	if err != nil {
		t.Fatalf("first LoadOrCreateIdentity: %v", err)
	}

	data, err := os.ReadFile(path)
	if err != nil {
		t.Fatalf("reading persisted file: %v", err)
	}
	data[10] ^= 0xFF // corrupt a byte inside the private key field
	if err := os.WriteFile(path, data, 0600); err != nil {
		t.Fatalf("writing corrupted file: %v", err)
	}

	id2, err := LoadOrCreateIdentity(path)
	if err != nil {
		t.Fatalf("LoadOrCreateIdentity on corrupt record: %v", err)
	}
	if id1.PublicKey() == id2.PublicKey() {
		t.Fatalf("corrupted record was accepted instead of triggering regeneration")
	}
}

func TestIdentityToleratesSingleCorruptCRCCopy(t *testing.T) {
	path := filepath.Join(t.TempDir(), "devicekey")

	id1, err := LoadOrCreateIdentity(path)
	if err != nil {
		t.Fatalf("first LoadOrCreateIdentity: %v", err)
	}

	data, err := os.ReadFile(path)
	if err != nil {
		t.Fatalf("reading persisted file: %v", err)
	}
	data[68] ^= 0xFF // corrupt only the first CRC copy, leave the duplicate intact
	if err := os.WriteFile(path, data, 0600); err != nil {
		t.Fatalf("writing file with one bad CRC copy: %v", err)
	}

	id2, err := LoadOrCreateIdentity(path)
	if err != nil {
		t.Fatalf("LoadOrCreateIdentity: %v", err)
	}
	if id1.PublicKey() != id2.PublicKey() {
		t.Fatalf("a single corrupted CRC copy should still be tolerated via the duplicate")
	}
}
