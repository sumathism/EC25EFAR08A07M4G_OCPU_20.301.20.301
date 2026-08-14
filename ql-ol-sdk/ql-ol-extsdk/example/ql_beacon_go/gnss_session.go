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

// GNSS is left running continuously and its cache read at send time,
// exactly like LAFV2's own EC25E handling (src/comm/comm_ec25e/lafm_comm_ec25e_gnss.c):
// that firmware sends AT+QGPS=1 once, then polls AT+QGPSGNMEA every few
// seconds, caching whatever fix comes back (lafm_app_dao_set_gnss_info) for
// the beacon builder to read later. This mirrors that shape using QL_LOC's
// async callback ("Usage 1" in ql_mcm_gps.h) instead of NMEA polling:
// enable, cache the latest fix as it arrives, read the cache at send time.
//
// Critically, LAFV2 also runs a watchdog (lafm_comm_ec25e_gnss_enabled_check)
// that disables and re-enables the GNSS engine (AT+QGPSEND then a fresh
// AT+QGPS=1) if it goes LAFM_CONFIG_COMM_EC25E_TIMEOUT_NO_FIX (5min) without
// a valid fix, or LAFM_CONFIG_COMM_EC25E_TIMEOUT_NO_VALID_TOKEN (1min)
// without even a valid NMEA response at all - a stuck GNSS engine (e.g.
// wedged after losing satellite lock) doesn't just sit there forever
// otherwise. gnssWatchdogLoop mirrors the detection side of that, but not
// the recovery action itself: LAFV2's restart works over raw AT commands
// that only touch the GPS *engine* (AT+QGPSEND/AT+QGPS=1), never the modem's
// QMI/USB connection. The QL_LOC C API's nearest equivalent is
// Stop_Navigation/Start_Navigation (see restartNavigation) - Client_Init/
// Client_Deinit/AddRxIndMsgHandler are a different, one-time-per-process
// layer (ql_mcm_gps.h documents exactly one Init->...->Deinit lifecycle, no
// handler-removal call, no supported repeated re-init). So the watchdog only
// detects and signals staleness on gnssStaleCh; main() decides whether/how
// to act on it (restartNavigation, in main.go), keeping that call-site
// decision out of this background goroutine.
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

	// Watchdog bookkeeping - see gnssWatchdogLoop. Stamped from the QL_LOC
	// callbacks (gnss_callback.go) under gnssMu, same as the rest of the cache.
	gnssLastValidFixAt   time.Time
	gnssLastValidTokenAt time.Time
)

// Timeouts match LAFV2's own EC25E config exactly
// (src/config/lafm_config_comm_ec25e.h) - see gnssWatchdogLoop.
const (
	gnssTimeoutNoFix         = 300 * time.Second
	gnssTimeoutNoValidToken  = 60 * time.Second
	gnssWatchdogPollInterval = 5 * time.Second
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

	// Grace period starts from a successful enable, not process start -
	// matches LAFV2's lafm_comm_ec25e_gnss_en_inprog_at_qgps_done(), which
	// seeds both timestamps to "now" the moment AT+QGPS? confirms enabled.
	gnssMu.Lock()
	gnssLastValidFixAt = gnssStartedAt
	gnssLastValidTokenAt = gnssStartedAt
	gnssMu.Unlock()

	return nil
}

func stopGNSSSession() {
	C.QL_LOC_Stop_Navigation(gnssHandle)
	C.QL_LOC_Client_Deinit(gnssHandle)
}

// restartNavigation is the recovery action gnssWatchdogLoop signals for -
// see main.go's handling of gnssStaleCh and the package doc comment above
// for why. Only touches Stop_Navigation/Set_Position_Mode/Start_Navigation
// on the existing gnssHandle - the nearest QL_LOC equivalent to LAFV2's
// AT+QGPSEND/AT+QGPS=1 engine restart. Deliberately does NOT call
// Client_Init/Client_Deinit/AddRxIndMsgHandler again.
func restartNavigation() error {
	C.QL_LOC_Stop_Navigation(gnssHandle)

	posMode := C.QL_LOC_POS_MODE_INFO_T{
		mode:               C.E_QL_LOC_POS_MODE_STANDALONE,
		recurrence:         C.E_QL_LOC_POS_RECURRENCE_PERIODIC,
		min_interval:       1000,
		preferred_accuracy: 50,
		preferred_time:     90,
	}
	if ret := C.QL_LOC_Set_Position_Mode(gnssHandle, &posMode); ret != C.E_QL_SUCCESS {
		return fmt.Errorf("QL_LOC_Set_Position_Mode failed: %d", ret)
	}

	restartedAt := time.Now()
	if ret := C.QL_LOC_Start_Navigation(gnssHandle); ret != C.E_QL_SUCCESS {
		return fmt.Errorf("QL_LOC_Start_Navigation failed: %d", ret)
	}

	gnssMu.Lock()
	gnssStartedAt = restartedAt // TTFF (gnss_callback.go) should measure from this restart, not the original start
	gnssGotFirstFix = false
	gnssLastValidFixAt = restartedAt
	gnssLastValidTokenAt = restartedAt
	gnssMu.Unlock()
	return nil
}

// gnssWatchdogLoop mirrors the detection side of LAFV2's
// lafm_comm_ec25e_gnss_enabled_check() (see the package doc comment above
// for why the recovery action lives in main() instead of here). Sends a
// reason string on staleCh (non-blocking, debounced so it fires once per
// stale period rather than every poll) when the GNSS engine has gone
// gnssTimeoutNoFix without a valid fix or gnssTimeoutNoValidToken without
// even a valid NMEA/location/SV response at all. Run this as a goroutine for
// the life of the process; it returns when stopCh is closed.
func gnssWatchdogLoop(stopCh <-chan struct{}, staleCh chan<- string) {
	ticker := time.NewTicker(gnssWatchdogPollInterval)
	defer ticker.Stop()

	signaledStale := false
	for {
		select {
		case <-stopCh:
			return
		case <-ticker.C:
			gnssMu.Lock()
			lastFix := gnssLastValidFixAt
			lastToken := gnssLastValidTokenAt
			gnssMu.Unlock()

			now := time.Now()
			staleNoFix := now.Sub(lastFix) >= gnssTimeoutNoFix
			staleNoToken := now.Sub(lastToken) >= gnssTimeoutNoValidToken
			if !staleNoFix && !staleNoToken {
				signaledStale = false
				continue
			}
			if signaledStale {
				continue // already signaled this stale period, don't spam
			}

			reason := "no valid NMEA/location response"
			if staleNoFix {
				reason = "no valid fix"
			}
			reason = fmt.Sprintf("%s for over %s (last fix %s ago, last token %s ago)",
				reason, gnssTimeoutNoValidToken, now.Sub(lastFix).Round(time.Second), now.Sub(lastToken).Round(time.Second))

			select {
			case staleCh <- reason:
				signaledStale = true
			default:
				// main() hasn't drained the last signal yet - try again next tick.
			}
		}
	}
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
