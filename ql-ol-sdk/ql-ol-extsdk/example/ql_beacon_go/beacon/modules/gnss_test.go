package modules

import (
	"encoding/binary"
	"math"
	"testing"
)

func TestGNSSInfoEncodeLayout(t *testing.T) {
	g := GNSSInfo{
		LastUpdatedEpoch: 1785000000,
		LastFixEpoch:     1784999990,
		TTFF:             3,
		FixQuality:       1,
		LatitudeDeg:      -12.917047, // South -> direction bit0 set
		LongitudeDeg:     77.648966,  // East -> direction bit1 clear
		AltitudeM:        828.10,
		HeadingDeg:       82.72,
		SpeedMPS:         2.74,
		NSatUsed:         8,
		PDOP:             1.3,
		HDOP:             1.0,
		VDOP:             0.8,
		UndulationM:      -85.0,
		SatInfo: []SatConstellationInfo{
			{ConstType: 1, NSatUsed: 8, NSatVisible: 13, AvgSNR: 20},
			{ConstType: 4, NSatUsed: 0, NSatVisible: 5, AvgSNR: 27},
		},
	}
	got := g.Encode()

	wantLen := 38 + 5*2
	if len(got) != wantLen {
		t.Fatalf("Encode() length = %d, want %d", len(got), wantLen)
	}

	if v := binary.LittleEndian.Uint32(got[0:4]); v != g.LastUpdatedEpoch {
		t.Errorf("last_updated_epoch = %d, want %d", v, g.LastUpdatedEpoch)
	}
	if v := binary.LittleEndian.Uint32(got[4:8]); v != g.LastFixEpoch {
		t.Errorf("last_fix_epoch = %d, want %d", v, g.LastFixEpoch)
	}
	if v := binary.LittleEndian.Uint16(got[8:10]); v != g.TTFF {
		t.Errorf("ttff = %d, want %d", v, g.TTFF)
	}
	if got[10] != g.FixQuality {
		t.Errorf("fix_quality = %d, want %d", got[10], g.FixQuality)
	}

	dir := got[11]
	if dir&(1<<0) == 0 {
		t.Errorf("expected South bit set for negative latitude, dir=%02x", dir)
	}
	if dir&(1<<1) != 0 {
		t.Errorf("expected West bit clear for positive longitude, dir=%02x", dir)
	}

	lat := binary.LittleEndian.Uint32(got[12:16])
	wantLat := uint32(12.917047 * 6_000_000)
	if diff := int64(lat) - int64(wantLat); diff < -1 || diff > 1 {
		t.Errorf("latitude encoded = %d, want ~%d", lat, wantLat)
	}

	lng := binary.LittleEndian.Uint32(got[16:20])
	wantLng := uint32(77.648966 * 6_000_000)
	if diff := int64(lng) - int64(wantLng); diff < -1 || diff > 1 {
		t.Errorf("longitude encoded = %d, want ~%d", lng, wantLng)
	}

	alt := int32(binary.LittleEndian.Uint32(got[20:24]))
	if alt != 82810 {
		t.Errorf("altitude encoded = %d, want 82810 (828.10m * 100)", alt)
	}

	heading := binary.LittleEndian.Uint16(got[24:26])
	if heading != 8272 {
		t.Errorf("heading encoded = %d, want 8272 (82.72deg * 100)", heading)
	}

	speedKmph := binary.LittleEndian.Uint16(got[26:28])
	wantSpeed := uint16(math.Round(2.74 * 3.6 * 100))
	if speedKmph != wantSpeed {
		t.Errorf("speed_kmph encoded = %d, want %d", speedKmph, wantSpeed)
	}

	if got[28] != g.NSatUsed {
		t.Errorf("nsat_used = %d, want %d", got[28], g.NSatUsed)
	}
	if v := binary.LittleEndian.Uint16(got[29:31]); v != 130 {
		t.Errorf("pdop = %d, want 130 (1.3 * 100)", v)
	}
	if v := binary.LittleEndian.Uint16(got[31:33]); v != 100 {
		t.Errorf("hdop = %d, want 100 (1.0 * 100)", v)
	}
	if v := binary.LittleEndian.Uint16(got[33:35]); v != 80 {
		t.Errorf("vdop = %d, want 80 (0.8 * 100)", v)
	}
	if v := int16(binary.LittleEndian.Uint16(got[35:37])); v != -8500 {
		t.Errorf("undulation = %d, want -8500 (-85.0m * 100)", v)
	}

	if got[37] != 2 {
		t.Fatalf("sat_count = %d, want 2", got[37])
	}
	gps := got[38:43]
	if gps[0] != 1 || gps[1] != 8 || gps[2] != 13 {
		t.Errorf("GPS sat block = %v, want [const=1 used=8 visible=13 ...]", gps[:3])
	}
	if v := binary.LittleEndian.Uint16(gps[3:5]); v != 20 {
		t.Errorf("GPS avg_snr = %d, want 20", v)
	}
	beidou := got[43:48]
	if beidou[0] != 4 || beidou[1] != 0 || beidou[2] != 5 {
		t.Errorf("BeiDou sat block = %v, want [const=4 used=0 visible=5 ...]", beidou[:3])
	}
	if v := binary.LittleEndian.Uint16(beidou[3:5]); v != 27 {
		t.Errorf("BeiDou avg_snr = %d, want 27", v)
	}
}

func TestGNSSInfoEncodeNoSatInfo(t *testing.T) {
	g := GNSSInfo{LatitudeDeg: 12.9, LongitudeDeg: 77.6}
	got := g.Encode()
	if len(got) != 38 {
		t.Fatalf("Encode() length = %d, want 38 with no SatInfo", len(got))
	}
	if got[37] != 0 {
		t.Errorf("sat_count = %d, want 0", got[37])
	}
}

func TestGNSSInfoNorthEastDirectionBits(t *testing.T) {
	g := GNSSInfo{LatitudeDeg: 12.9, LongitudeDeg: 77.6}
	got := g.Encode()
	if got[11] != 0 {
		t.Errorf("expected direction byte 0 for North+East, got %02x", got[11])
	}
}
