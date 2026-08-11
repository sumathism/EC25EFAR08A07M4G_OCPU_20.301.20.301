// Package nmea parses the handful of NMEA sentences QL_LOC delivers via its
// LOC_IND_NMEA_INFO_ON indication that have no structured API equivalent:
// $..GSA (satellites used in fix + PDOP/HDOP/VDOP) and $..GGA (fix quality
// digit, total satellites used, geoid undulation). This mirrors LAFV2's own
// comm_ec25e_gnss_parser.c, which gets this same data the same way (over
// AT+QGPSGNMEA rather than QL_LOC, but the NMEA text itself is identical).
package nmea

import (
	"strconv"
	"strings"
)

// GSA is a parsed $..GSA sentence.
type GSA struct {
	Talker string // "GP", "GL", "GA", "GB"/"BD"/"PQ" (BeiDou)
	NPRN   uint8
	PDOP   float64
	HDOP   float64
	VDOP   float64
}

// GGA is a parsed $..GGA sentence.
type GGA struct {
	GPSQuality  uint8
	NSatUsed    uint8
	UndulationM float64
}

// TalkerToConstBit maps a GSA/GGA talker prefix to LAFV2's own constellation
// bit positions (lafm_app_beacon_server_fill_beacon_gnss_info: bit0=GPS,
// bit1=GLONASS, bit2=Galileo, bit3=BeiDou). Constellations LAFV2 doesn't
// track (SBAS, QZSS, IRNSS) are intentionally absent.
var TalkerToConstBit = map[string]int{
	"GP": 0,
	"GL": 1,
	"GA": 2,
	"GB": 3,
	"BD": 3,
	"PQ": 3, // LAFV2's own BeiDou GSA naming (LAFM_COMM_GNSS_NMEA_PQGSA)
}

// stripChecksum verifies the trailing "*HH" NMEA checksum (XOR of every byte
// between '$' and '*') and returns the sentence body without it. Returns
// ok=false on a missing/mismatched checksum rather than parsing untrusted
// data.
func stripChecksum(sentence string) (body string, ok bool) {
	sentence = strings.TrimSpace(sentence)
	if len(sentence) < 4 || sentence[0] != '$' {
		return "", false
	}
	star := strings.LastIndexByte(sentence, '*')
	if star < 0 || star+3 > len(sentence) {
		return "", false
	}
	want, err := strconv.ParseUint(sentence[star+1:star+3], 16, 8)
	if err != nil {
		return "", false
	}
	var got byte
	for i := 1; i < star; i++ {
		got ^= sentence[i]
	}
	if got != byte(want) {
		return "", false
	}
	return sentence[1:star], true
}

// parseFloatOrZero parses a numeric NMEA field, treating a blank field (as
// seen in, e.g., a no-fix "$GPGSA,A,1,,,,,,,,,,,,,,,,*1E" - PDOP/HDOP/VDOP
// all empty) as 0 rather than a parse error - a blank optional field is
// valid NMEA, not corruption.
func parseFloatOrZero(f string) (float64, bool) {
	if f == "" {
		return 0, true
	}
	v, err := strconv.ParseFloat(f, 64)
	return v, err == nil
}

func parseUintOrZero(f string, bitSize int) (uint64, bool) {
	if f == "" {
		return 0, true
	}
	v, err := strconv.ParseUint(f, 10, bitSize)
	return v, err == nil
}

// ParseGSA parses a $..GSA sentence: talker,mode,fixtype,prn1..prn12,pdop,
// hdop,vdop. PDOP/HDOP/VDOP are blank in a no-fix sentence - that's not a
// parse failure, it just means NPRN will be 0 too.
func ParseGSA(sentence string) (GSA, bool) {
	body, ok := stripChecksum(sentence)
	if !ok || len(body) < 5 || body[2:5] != "GSA" {
		return GSA{}, false
	}
	fields := strings.Split(body, ",")
	if len(fields) < 18 {
		return GSA{}, false
	}

	var nprn uint8
	for _, f := range fields[3:15] {
		if f != "" {
			nprn++
		}
	}

	pdop, ok1 := parseFloatOrZero(fields[15])
	hdop, ok2 := parseFloatOrZero(fields[16])
	vdop, ok3 := parseFloatOrZero(fields[17])
	if !ok1 || !ok2 || !ok3 {
		return GSA{}, false
	}

	return GSA{Talker: body[0:2], NPRN: nprn, PDOP: pdop, HDOP: hdop, VDOP: vdop}, true
}

// ParseGGA parses a $..GGA sentence: time,lat,ns,lon,ew,quality,nsat,hdop,
// alt,M,undulation,M,... nsat/altitude/undulation are blank in a no-fix
// sentence - that's not a parse failure.
func ParseGGA(sentence string) (GGA, bool) {
	body, ok := stripChecksum(sentence)
	if !ok || len(body) < 5 || body[2:5] != "GGA" {
		return GGA{}, false
	}
	fields := strings.Split(body, ",")
	if len(fields) < 12 {
		return GGA{}, false
	}

	quality, ok1 := parseUintOrZero(fields[6], 8)
	nsat, ok2 := parseUintOrZero(fields[7], 8)
	undulation, ok3 := parseFloatOrZero(fields[11])
	if !ok1 || !ok2 || !ok3 {
		return GGA{}, false
	}

	return GGA{GPSQuality: uint8(quality), NSatUsed: uint8(nsat), UndulationM: undulation}, true
}
