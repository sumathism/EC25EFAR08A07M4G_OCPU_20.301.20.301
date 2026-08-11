package main

/*
#include <ql_oe.h>
#include "ql_mcm_gps.h"

extern void goLocCallback(loc_client_handle_type h_loc, E_QL_LOC_NFY_MSG_ID_T e_msg_id, void *pv_data, void *contextPtr);

static int ql_beacon_register_loc_callback(void) {
    return QL_LOC_AddRxIndMsgHandler(goLocCallback, NULL);
}
*/
import "C"

import (
	"fmt"
	"sync"
	"time"

	"ql_beacon/beacon/modules"
	"ql_beacon/nmea"
)

// GNSS is enabled once for the life of the process and left running
// continuously, exactly like LAFV2's own EC25E handling
// (src/comm/comm_ec25e/lafm_comm_ec25e_gnss.c): that firmware sends
// AT+QGPS=1 once, then polls AT+QGPSGNMEA every few seconds indefinitely,
// caching whatever fix comes back (lafm_app_dao_set_gnss_info) for the
// beacon builder to read later - it never stops/restarts the GNSS engine
// per beacon. This mirrors that shape using QL_LOC's async callback
// ("Usage 1" in ql_mcm_gps.h) instead of NMEA polling: enable once, cache
// the latest fix as it arrives, read the cache at send time.
//
// Three independent QL_LOC indications feed the cache, each covering what
// the others can't:
//   - LOCATION_INFO: lat/lon/altitude/heading/speed (structured, reliable).
//   - SV_INFO: satellites visible + SNR per constellation - confirmed via
//     test_sv_info against live $GPGSV ground truth to deliver
//     QL_LOC_GNSS_SV_INFO_T (svid+constellation+c_n0_dbhz), not the older
//     QL_LOC_SV_INFO_T (whose "prn" field the same test proved reads as
//     garbage - e.g. 65537 instead of a real PRN).
//   - NMEA_INFO: QL_LOC has no structured API for fix-quality digit,
//     satellites-used, PDOP/HDOP/VDOP, or geoid undulation at all - these
//     only exist in $..GSA/$..GGA text, parsed by the nmea package,
//     mirroring LAFV2's own comm_ec25e_gnss_parser.c (which gets this same
//     data the same way, just over AT+QGPSGNMEA instead of QL_LOC).
//
// Constellation bit positions throughout this cache match LAFV2's own
// convention (lafm_app_beacon_server_fill_beacon_gnss_info): 0=GPS,
// 1=GLONASS, 2=Galileo, 3=BeiDou - see satConstType and svConstToBit below.
var (
	gnssMu          sync.Mutex
	gnssCached      modules.GNSSInfo
	gnssStartedAt   time.Time
	gnssGotFirstFix bool
	gnssHandle      C.loc_client_handle_type

	gnssGSA      [4]nmea.GSA
	gnssGSAValid [4]bool

	gnssSVVisible [4]uint8
	gnssSVAvgSNR  [4]uint16
	gnssSVValid   [4]bool
)

// satConstType is GNSS_INFO_38's own wire encoding for each constellation
// bit position (lafm_beacon_server.c: GPS=1, GLONASS=2, Galileo=3, BeiDou=4).
var satConstType = [4]uint8{1, 2, 3, 4}

// svConstToBit maps QL_LOC_GNSS_SV_INFO_T.constellation (Android's
// GnssConstellationType convention - confirmed via test_sv_info: GPS=1,
// QZSS=4, BeiDou=5) to LAFV2's own bit positions. SBAS/QZSS/IRNSS aren't
// tracked by LAFV2's beacon at all, so they're intentionally absent here.
var svConstToBit = map[int8]int{
	1: 0, // GPS
	3: 1, // GLONASS
	6: 2, // Galileo
	5: 3, // BeiDou
}

// buildSatInfo merges the GSA cache (satellites used) with the SV_INFO cache
// (satellites visible + SNR) into GNSS_INFO_38's satellite block. Must be
// called with gnssMu held.
func buildSatInfo() []modules.SatConstellationInfo {
	var out []modules.SatConstellationInfo
	for bit := 0; bit < 4; bit++ {
		if !gnssGSAValid[bit] && !gnssSVValid[bit] {
			continue
		}
		var nsatUsed uint8
		if gnssGSAValid[bit] {
			nsatUsed = gnssGSA[bit].NPRN
		}
		out = append(out, modules.SatConstellationInfo{
			ConstType:   satConstType[bit],
			NSatUsed:    nsatUsed,
			NSatVisible: gnssSVVisible[bit],
			AvgSNR:      gnssSVAvgSNR[bit],
		})
	}
	return out
}

// startGNSSSession brings up the GNSS engine once at process startup. Call
// stopGNSSSession only at shutdown - see the package doc comment above.
func startGNSSSession() error {
	if ret := C.QL_LOC_Client_Init(&gnssHandle); ret != C.E_QL_SUCCESS {
		return fmt.Errorf("QL_LOC_Client_Init failed: %d", ret)
	}
	if ret := C.ql_beacon_register_loc_callback(); ret != C.E_QL_SUCCESS {
		return fmt.Errorf("QL_LOC_AddRxIndMsgHandler failed: %d", ret)
	}
	bitmask := C.int(C.LOC_IND_LOCATION_INFO_ON | C.LOC_IND_SV_INFO_ON | C.LOC_IND_NMEA_INFO_ON)
	if ret := C.QL_LOC_Set_Indications(gnssHandle, bitmask); ret != C.E_QL_SUCCESS {
		return fmt.Errorf("QL_LOC_Set_Indications failed: %d", ret)
	}

	// Required before Start_Navigation - confirmed against the SDK's own
	// example/gnss/example_gps.c (get_gps_info_by_cb), which sets this
	// exact mode/recurrence/interval/accuracy/timeout before starting
	// continuous navigation, and does get real fixes. Skipping this call
	// (as an earlier version of this code did) left the position-mode
	// request completely unset, and no fix was ever delivered.
	posMode := C.QL_LOC_POS_MODE_INFO_T{
		mode:               C.E_QL_LOC_POS_MODE_STANDALONE,
		recurrence:         C.E_QL_LOC_POS_RECURRENCE_PERIODIC,
		min_interval:       1000, // report frequency: 1Hz
		preferred_accuracy: 50,   // meters
		preferred_time:     90,   // seconds
	}
	if ret := C.QL_LOC_Set_Position_Mode(gnssHandle, &posMode); ret != C.E_QL_SUCCESS {
		return fmt.Errorf("QL_LOC_Set_Position_Mode failed: %d", ret)
	}

	gnssStartedAt = time.Now()
	if ret := C.QL_LOC_Start_Navigation(gnssHandle); ret != C.E_QL_SUCCESS {
		return fmt.Errorf("QL_LOC_Start_Navigation failed: %d", ret)
	}
	return nil
}

func stopGNSSSession() {
	C.QL_LOC_Stop_Navigation(gnssHandle)
	C.QL_LOC_Client_Deinit(gnssHandle)
}

// getGNSSInfo returns the latest cached fix, with SatInfo assembled from the
// SV_INFO/GSA caches - it never talks to the modem itself, see
// startGNSSSession.
func getGNSSInfo() modules.GNSSInfo {
	gnssMu.Lock()
	defer gnssMu.Unlock()
	info := gnssCached
	info.SatInfo = buildSatInfo()
	return info
}
