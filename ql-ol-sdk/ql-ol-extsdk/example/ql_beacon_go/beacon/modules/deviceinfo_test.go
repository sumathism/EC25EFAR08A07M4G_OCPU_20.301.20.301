package modules

import (
	"bytes"
	"testing"
)

// TestPubKeyIsRawNotHexEncoded guards against a real bug that shipped once:
// PubKey was being hex-encoded to an ASCII string before being placed in the
// TLV value, so the wire bytes were the 64 ASCII characters of the hex
// string rather than the 32 raw key bytes - the debug page then hex-encoded
// those 64 bytes again for display, producing a garbled 128-character
// double-hex-encoded result instead of the expected 64-character one.
func TestPubKeyIsRawNotHexEncoded(t *testing.T) {
	rawKey := make([]byte, 32)
	for i := range rawKey {
		rawKey[i] = byte(i) // distinctive, non-ASCII-safe pattern (bytes >0x7f included)
	}

	info := DeviceInfo{PubKey: rawKey}
	encoded := info.Encode()

	// nDevInfo(1B)=1, then key(1B)+len(1B)+value(32B)
	want := append([]byte{1, KeyPubKey, 32}, rawKey...)
	if !bytes.Equal(encoded, want) {
		t.Fatalf("Encode() = % x\nwant     = % x", encoded, want)
	}
}
