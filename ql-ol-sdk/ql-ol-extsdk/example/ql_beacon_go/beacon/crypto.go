package beacon

import (
	"crypto/rand"
	"encoding/hex"
	"errors"
	"os"
	"path/filepath"

	"golang.org/x/crypto/chacha20poly1305"
	"golang.org/x/crypto/curve25519"
)

// ServerPublicKeyHex is Intellicar's static X25519 public key for the beacon
// channel (LAFM_CONFIG_CRYPTO_SERVER_PUBLIC_KEY in the firmware; the trailing
// hex digit there is a vestigial typo - only the first 64 hex chars/32 bytes
// are ever used).
const ServerPublicKeyHex = "5C405ACF276528F63306336843D2206B9ACCE9C0B961F8176EDAFF5B2ABAE034"

const secureAAD = "LAFM"

// Identity is the device's own X25519 keypair used for the beacon ECDH
// exchange. Unlike an ephemeral per-session key, this is generated once and
// persisted to disk - the firmware does the same (a static key surviving
// reboots), and the server side likely keys device identity off this public
// key, so it must stay stable.
type Identity struct {
	private [32]byte
	public  [32]byte
}

// Persisted key record format, matching the firmware's lafm_crypto_keys_t
// flash record exactly (minus the unused server/Ed25519 fields we don't
// carry): "LAFM"(4B magic) + private_key(32B) + public_key(32B) + crc16(2B)
// + crc16(2B, duplicate copy - tolerates a torn write where only one CRC
// copy made it to disk). Both keys are generated together, once, and always
// read back verbatim - the public key is never re-derived on load.
const (
	keyMagic      = "LAFM"
	keyRecordSize = 4 + 32 + 32 + 2 + 2
)

// LoadOrCreateIdentity reads a persisted keypair from path, or generates and
// persists a new one (once) if none exists yet or the record is corrupt.
func LoadOrCreateIdentity(path string) (*Identity, error) {
	if id, ok := loadIdentity(path); ok {
		return id, nil
	}
	return generateAndPersistIdentity(path)
}

func loadIdentity(path string) (*Identity, bool) {
	data, err := os.ReadFile(path)
	if err != nil || len(data) != keyRecordSize || string(data[0:4]) != keyMagic {
		return nil, false
	}

	covered := data[0 : 4+64] // magic + private + public
	crc1 := getU16LE(data[68:70])
	crc2 := getU16LE(data[70:72])
	if computed := crc16(covered); computed != crc1 && computed != crc2 {
		return nil, false
	}

	id := &Identity{}
	copy(id.private[:], data[4:36])
	copy(id.public[:], data[36:68])
	return id, true
}

func generateAndPersistIdentity(path string) (*Identity, error) {
	id := &Identity{}
	if _, err := rand.Read(id.private[:]); err != nil {
		return nil, err
	}
	pub, err := curve25519.X25519(id.private[:], curve25519.Basepoint)
	if err != nil {
		return nil, err
	}
	copy(id.public[:], pub)

	record := make([]byte, 0, keyRecordSize)
	record = append(record, keyMagic...)
	record = append(record, id.private[:]...)
	record = append(record, id.public[:]...)
	crcBytes := make([]byte, 2)
	putU16LE(crcBytes, crc16(record))
	record = append(record, crcBytes...)
	record = append(record, crcBytes...) // duplicate copy, same as the firmware

	if err := os.MkdirAll(filepath.Dir(path), 0755); err != nil {
		return nil, err
	}
	if err := os.WriteFile(path, record, 0600); err != nil {
		return nil, err
	}
	return id, nil
}

// PublicKey returns the device's X25519 public key, sent to the server as
// part of the login handshake.
func (id *Identity) PublicKey() [32]byte {
	return id.public
}

// SharedSecret computes the ECDH shared secret against a peer public key
// (the server's, in practice).
func (id *Identity) SharedSecret(peerPublic [32]byte) ([32]byte, error) {
	var shared [32]byte
	out, err := curve25519.X25519(id.private[:], peerPublic[:])
	if err != nil {
		return shared, err
	}
	copy(shared[:], out)
	return shared, nil
}

// ServerPublicKey decodes the compiled-in server public key constant.
func ServerPublicKey() ([32]byte, error) {
	var pub [32]byte
	b, err := hex.DecodeString(ServerPublicKeyHex)
	if err != nil {
		return pub, err
	}
	if len(b) < 32 {
		return pub, errors.New("server public key constant is too short")
	}
	copy(pub[:], b[:32])
	return pub, nil
}

// buildChaChaCipherModule encrypts plaintext (a fully-built inner beacon
// frame) into the wire layout the firmware uses for module 2 of a type-102
// secure beacon: AAD_len(1B) + AAD(xB) + cipher(xB) + MAC(16B) + nonce(12B).
//
// The firmware reads its nonce from uninitialized stack memory (no RNG call
// anywhere in that code path) - that isn't replicated here. This uses a
// proper crypto/rand nonce per message instead.
func buildChaChaCipherModule(sharedSecret [32]byte, plaintext []byte) ([]byte, error) {
	aead, err := chacha20poly1305.New(sharedSecret[:])
	if err != nil {
		return nil, err
	}
	nonce := make([]byte, chacha20poly1305.NonceSize)
	if _, err := rand.Read(nonce); err != nil {
		return nil, err
	}
	sealed := aead.Seal(nil, nonce, plaintext, []byte(secureAAD)) // cipher || 16-byte MAC

	payload := make([]byte, 0, 1+len(secureAAD)+len(sealed)+len(nonce))
	payload = append(payload, byte(len(secureAAD)))
	payload = append(payload, secureAAD...)
	payload = append(payload, sealed...)
	payload = append(payload, nonce...)
	return payload, nil
}
