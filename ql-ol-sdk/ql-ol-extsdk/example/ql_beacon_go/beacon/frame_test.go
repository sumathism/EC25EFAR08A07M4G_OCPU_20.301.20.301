package beacon

import (
	"bytes"
	"io"
	"testing"
)

// byteAtATimePipe writes data one byte at a time on a goroutine, simulating
// a slow/fragmented socket read, and returns the read side (plus the write
// side, so the caller can close it once done).
func byteAtATimePipe(data []byte) (io.ReadCloser, io.WriteCloser) {
	pr, pw := io.Pipe()
	go func() {
		for _, b := range data {
			pw.Write([]byte{b})
		}
	}()
	return pr, pw
}

func TestBuildAndParseRoundTrip(t *testing.T) {
	b := NewBeacon(TypeSecure, 42)
	b.AddModule(ModuleLoginPubKey, []byte("hello-pubkey-bytes"))
	b.AddModule(ModuleBeaconAcks, []byte{2, 5, 0, 9, 0}) // count=2, seq 5, seq 9
	frame := b.Finish()

	parsed, consumed, ok := tryParseFrame(frame)
	if !ok {
		t.Fatalf("tryParseFrame did not accept a freshly-built frame")
	}
	if consumed != len(frame) {
		t.Fatalf("consumed=%d, want %d (full frame)", consumed, len(frame))
	}
	if parsed.BeaconType != TypeSecure {
		t.Errorf("BeaconType = %d, want %d", parsed.BeaconType, TypeSecure)
	}
	if parsed.SeqID != 42 {
		t.Errorf("SeqID = %d, want 42", parsed.SeqID)
	}
	if len(parsed.Modules) != 2 {
		t.Fatalf("got %d modules, want 2", len(parsed.Modules))
	}
	if payload, ok := parsed.Module(ModuleLoginPubKey); !ok || !bytes.Equal(payload, []byte("hello-pubkey-bytes")) {
		t.Errorf("ModuleLoginPubKey payload = %q, ok=%v", payload, ok)
	}
	acksPayload, ok := parsed.Module(ModuleBeaconAcks)
	if !ok {
		t.Fatalf("ModuleBeaconAcks not found")
	}
	acks := ParseBeaconAcks(acksPayload)
	if len(acks) != 2 || acks[0] != 5 || acks[1] != 9 {
		t.Errorf("ParseBeaconAcks = %v, want [5 9]", acks)
	}
}

func TestFrameReaderAcrossPartialReads(t *testing.T) {
	b := NewBeacon(TypeFromDevice, 7)
	b.AddModule(ModuleDeviceInfo, []byte{0}) // count=0 device-info entries
	frame := b.Finish()

	// Feed the reader one byte at a time, simulating a slow/fragmented socket.
	pr, pw := byteAtATimePipe(frame)
	fr := NewFrameReader(pr)

	parsed, err := fr.ReadFrame()
	if err != nil {
		t.Fatalf("ReadFrame: %v", err)
	}
	if parsed.SeqID != 7 || parsed.BeaconType != TypeFromDevice {
		t.Errorf("got type=%d seq=%d, want type=%d seq=7", parsed.BeaconType, parsed.SeqID, TypeFromDevice)
	}
	pw.Close()
}

func TestFrameReaderSkipsGarbagePrefix(t *testing.T) {
	b := NewBeacon(TypeSecure, 1)
	b.AddModule(ModuleBeaconAcks, []byte{0})
	good := b.Finish()

	garbage := append([]byte("XX not a frame LA5-but-not-really"), good...)
	fr := NewFrameReader(bytes.NewReader(garbage))

	parsed, err := fr.ReadFrame()
	if err != nil {
		t.Fatalf("ReadFrame: %v", err)
	}
	if parsed.SeqID != 1 {
		t.Errorf("SeqID = %d, want 1 (failed to resync past garbage prefix)", parsed.SeqID)
	}
}

func TestTryParseFrameRejectsCorruptedCRC(t *testing.T) {
	b := NewBeacon(TypeFromDevice, 3)
	b.AddModule(ModuleDeviceInfo, []byte{0})
	frame := b.Finish()
	frame[len(frame)-1] ^= 0xFF // corrupt one CRC byte

	_, consumed, ok := tryParseFrame(frame)
	if ok {
		t.Fatalf("tryParseFrame accepted a frame with a corrupted CRC")
	}
	if consumed != 1 {
		t.Errorf("consumed = %d, want 1 (should drop one byte to resync)", consumed)
	}
}
