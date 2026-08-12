package nmea

import "testing"

// Real sentences captured live from test_sv_info on the actual EC25 device.
func TestParseGSARealSentence(t *testing.T) {
	gsa, ok := ParseGSA("$GPGSA,A,3,01,03,10,16,26,28,31,32,,,,,1.3,1.0,0.8*30")
	if !ok {
		t.Fatal("ParseGSA returned ok=false")
	}
	if gsa.Talker != "GP" {
		t.Errorf("Talker = %q, want GP", gsa.Talker)
	}
	if gsa.NPRN != 8 {
		t.Errorf("NPRN = %d, want 8", gsa.NPRN)
	}
	if gsa.PDOP != 1.3 || gsa.HDOP != 1.0 || gsa.VDOP != 0.8 {
		t.Errorf("PDOP/HDOP/VDOP = %v/%v/%v, want 1.3/1.0/0.8", gsa.PDOP, gsa.HDOP, gsa.VDOP)
	}
}

func TestParseGSABadChecksum(t *testing.T) {
	if _, ok := ParseGSA("$GPGSA,A,3,01,03,10,16,26,28,31,32,,,,,1.3,1.0,0.8*FF"); ok {
		t.Error("expected ok=false for a corrupted checksum")
	}
}

func TestParseGGARealSentence(t *testing.T) {
	gga, ok := ParseGGA("$GPGGA,112035.00,1255.022507,N,07738.938189,E,1,08,1.0,907.6,M,-85.0,M,,*41")
	if !ok {
		t.Fatal("ParseGGA returned ok=false")
	}
	if gga.GPSQuality != 1 {
		t.Errorf("GPSQuality = %d, want 1", gga.GPSQuality)
	}
	if gga.NSatUsed != 8 {
		t.Errorf("NSatUsed = %d, want 8", gga.NSatUsed)
	}
	if gga.UndulationM != -85.0 {
		t.Errorf("UndulationM = %v, want -85.0", gga.UndulationM)
	}
}

func TestParseGGABadChecksum(t *testing.T) {
	if _, ok := ParseGGA("$GPGGA,112035.00,1255.022507,N,07738.938189,E,1,08,1.0,907.6,M,-85.0,M,,*00"); ok {
		t.Error("expected ok=false for a corrupted checksum")
	}
}

func TestParseGSAWrongSentenceType(t *testing.T) {
	if _, ok := ParseGSA("$GPGGA,112035.00,1255.022507,N,07738.938189,E,1,08,1.0,907.6,M,-85.0,M,,*41"); ok {
		t.Error("expected ok=false when handed a GGA sentence")
	}
}

func TestParseGSAEmptyPRNList(t *testing.T) {
	gsa, ok := ParseGSA("$GLGSA,A,1,,,,,,,,,,,,,0.0,0.0,0.0*2C")
	if !ok {
		t.Fatal("ParseGSA returned ok=false")
	}
	if gsa.NPRN != 0 {
		t.Errorf("NPRN = %d, want 0", gsa.NPRN)
	}
	if gsa.Talker != "GL" {
		t.Errorf("Talker = %q, want GL", gsa.Talker)
	}
}

// A real no-fix GSA: PDOP/HDOP/VDOP are blank, not "0.0" - this must not be
// treated as a parse failure (a live device with too few visible satellites
// sends exactly this, and previously the whole sentence - including the
// correct NPRN=0 - was silently discarded because of it).
func TestParseGSANoFixBlankDOP(t *testing.T) {
	gsa, ok := ParseGSA("$GPGSA,A,1,,,,,,,,,,,,,,,,*32")
	if !ok {
		t.Fatal("ParseGSA returned ok=false for a blank-DOP no-fix sentence")
	}
	if gsa.NPRN != 0 {
		t.Errorf("NPRN = %d, want 0", gsa.NPRN)
	}
	if gsa.PDOP != 0 || gsa.HDOP != 0 || gsa.VDOP != 0 {
		t.Errorf("PDOP/HDOP/VDOP = %v/%v/%v, want 0/0/0", gsa.PDOP, gsa.HDOP, gsa.VDOP)
	}
}

// A real no-fix GGA: nsat/hdop/altitude/undulation are all blank.
func TestParseGGANoFixBlankFields(t *testing.T) {
	gga, ok := ParseGGA("$GPGGA,,,,,,0,,,,,,,*4A")
	if !ok {
		t.Fatal("ParseGGA returned ok=false for a blank-field no-fix sentence")
	}
	if gga.GPSQuality != 0 {
		t.Errorf("GPSQuality = %d, want 0", gga.GPSQuality)
	}
	if gga.NSatUsed != 0 {
		t.Errorf("NSatUsed = %d, want 0", gga.NSatUsed)
	}
	if gga.UndulationM != 0 {
		t.Errorf("UndulationM = %v, want 0", gga.UndulationM)
	}
}
