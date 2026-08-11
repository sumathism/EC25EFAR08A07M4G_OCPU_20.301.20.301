package main

/*
#include <ql_oe.h>
#include "ql_mcm_gps.h"
*/
import "C"

import (
	"time"
	"unsafe"

	"ql_beacon/nmea"
)

// goLocCallback is QL_LOC's async fix handler (registered by
// startGNSSSession in gnss_session.go) - see that file's doc comment for the
// full picture of which indication feeds which field.
//
//export goLocCallback
func goLocCallback(hLoc C.loc_client_handle_type, eMsgID C.E_QL_LOC_NFY_MSG_ID_T, pvData unsafe.Pointer, contextPtr unsafe.Pointer) {
	switch eMsgID {
	case C.E_QL_LOC_NFY_MSG_ID_LOCATION_INFO:
		handleLocationInfo((*C.QL_LOC_LOCATION_INFO_T)(pvData))
	case C.E_QL_LOC_NFY_MSG_ID_SV_INFO:
		handleSVInfo((*C.QL_LOC_GNSS_SV_STATUS_T)(pvData))
	case C.E_QL_LOC_NFY_MSG_ID_NMEA_INFO:
		handleNMEAInfo((*C.QL_LOC_NMEA_INFO_T)(pvData))
	}
}

// handleLocationInfo updates lat/lon/altitude/heading/speed. Every field is
// only updated when its corresponding E_QL_LOC_LOCATION_*_VALID flag is set,
// matching LAFV2's own gnrmc/gngga validity checks before trusting a field.
func handleLocationInfo(loc *C.QL_LOC_LOCATION_INFO_T) {
	flags := uint32(loc.flags)

	gnssMu.Lock()
	defer gnssMu.Unlock()

	// LAFV2 stamps both last_updated_at_epoch and last_fix_at_epoch from a
	// single lafm_timer_get_epoch_s() call per event (comm_ec25e_gnss.c) -
	// one clock, read once, used for both fields. That's also why "GPS
	// time" and "comms time" always agree on those boards: they're never
	// two different clocks. Do the same here - one time.Now() call, used
	// for both - rather than trusting QL_LOC_LOCATION_INFO_T.timestamp
	// (the modem's own internal clock, a separate domain from the Linux
	// host clock, and observed stale/stuck on this hardware).
	now := uint32(time.Now().Unix())
	gnssCached.LastUpdatedEpoch = now

	if flags&uint32(C.E_QL_LOC_LOCATION_LAT_LONG_VALID) != 0 {
		gnssCached.LastFixEpoch = now
		gnssCached.LatitudeDeg = float64(loc.latitude)
		gnssCached.LongitudeDeg = float64(loc.longitude)
		if !gnssGotFirstFix {
			gnssGotFirstFix = true
			gnssCached.TTFF = uint16(time.Since(gnssStartedAt).Seconds())
		}
	}
	if flags&uint32(C.E_QL_LOC_LOCATION_ALTITUDE_VALID) != 0 {
		gnssCached.AltitudeM = float64(loc.altitude)
	}
	if flags&uint32(C.E_QL_LOC_LOCATION_BEARING_VALID) != 0 {
		gnssCached.HeadingDeg = float64(loc.bearing)
	}
	if flags&uint32(C.E_QL_LOC_LOCATION_SPEED_VALID) != 0 {
		gnssCached.SpeedMPS = float64(loc.speed)
	}
}

// handleSVInfo rebuilds the per-constellation visible-count/avg-SNR cache
// from scratch on every event - QL_LOC delivers a complete snapshot each
// time (confirmed via test_sv_info), not an incremental update, so stale
// entries from a previous snapshot must not linger.
//
// Struct layout confirmed empirically: QL_LOC_GNSS_SV_INFO_T
// (svid/constellation/c_n0_dbhz), not the older QL_LOC_SV_INFO_T - see
// gnss_session.go's doc comment.
func handleSVInfo(sv *C.QL_LOC_GNSS_SV_STATUS_T) {
	var visible [4]uint8
	var snrSum [4]uint32
	var snrCount [4]uint8

	numSVs := int(sv.num_svs)
	if numSVs > 32 {
		numSVs = 32
	}
	for i := 0; i < numSVs; i++ {
		entry := sv.sv_list[i]
		bit, ok := svConstToBit[int8(entry.constellation)]
		if !ok {
			continue // SBAS/QZSS/IRNSS - not tracked by LAFV2's beacon
		}
		visible[bit]++
		if snr := float32(entry.c_n0_dbhz); snr > 0 {
			snrSum[bit] += uint32(snr)
			snrCount[bit]++
		}
	}

	gnssMu.Lock()
	defer gnssMu.Unlock()
	for bit := 0; bit < 4; bit++ {
		gnssSVValid[bit] = visible[bit] > 0
		gnssSVVisible[bit] = visible[bit]
		if snrCount[bit] > 0 {
			gnssSVAvgSNR[bit] = uint16(snrSum[bit] / uint32(snrCount[bit]))
		} else {
			gnssSVAvgSNR[bit] = 0
		}
	}
}

// handleNMEAInfo parses $..GSA/$..GGA sentences - see gnss_session.go's doc
// comment for why (QL_LOC has no structured API for any of this).
func handleNMEAInfo(n *C.QL_LOC_NMEA_INFO_T) {
	sentence := C.GoString(&n.nmea[0])

	if gsa, ok := nmea.ParseGSA(sentence); ok {
		bit, known := nmea.TalkerToConstBit[gsa.Talker]
		if !known {
			return
		}
		gnssMu.Lock()
		gnssGSA[bit] = gsa
		gnssGSAValid[bit] = true
		if bit == 0 { // GPS - matches LAFV2's use of gngsa_1 specifically for the top-level DOP fields
			gnssCached.PDOP = gsa.PDOP
			gnssCached.HDOP = gsa.HDOP
			gnssCached.VDOP = gsa.VDOP
		}
		gnssMu.Unlock()
		return
	}

	if gga, ok := nmea.ParseGGA(sentence); ok {
		gnssMu.Lock()
		gnssCached.FixQuality = gga.GPSQuality
		gnssCached.NSatUsed = gga.NSatUsed
		gnssCached.UndulationM = gga.UndulationM
		gnssMu.Unlock()
	}
}
