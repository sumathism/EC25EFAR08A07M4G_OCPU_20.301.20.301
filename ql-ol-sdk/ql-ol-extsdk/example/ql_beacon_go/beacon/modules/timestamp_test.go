package modules

import (
	"encoding/binary"
	"testing"
)

func TestTimestampEncodeLayout(t *testing.T) {
	ts := Timestamp{UptimeMs: 18156840000, UTCTimeMs: 1785228385000}
	got := ts.Encode()

	if len(got) != 16 {
		t.Fatalf("Encode() length = %d, want 16", len(got))
	}
	if v := binary.LittleEndian.Uint64(got[0:8]); v != ts.UptimeMs {
		t.Errorf("uptime_ms = %d, want %d", v, ts.UptimeMs)
	}
	if v := binary.LittleEndian.Uint64(got[8:16]); v != ts.UTCTimeMs {
		t.Errorf("utctime_ms = %d, want %d", v, ts.UTCTimeMs)
	}
}
