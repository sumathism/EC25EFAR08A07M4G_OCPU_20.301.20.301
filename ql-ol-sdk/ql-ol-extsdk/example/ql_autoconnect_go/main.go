// ql_autoconnect brings up cellular data automatically at boot: waits for the
// SIM, waits for network registration, then starts and maintains a data call
// using Quectel's MCM/dsi_netctrl APIs.
package main

/*
#cgo CFLAGS: -march=armv7-a -mfloat-abi=softfp -mfpu=neon
#include <stdlib.h>
#include <ql_oe.h>
#include "ql_mcm_sim.h"
#include "ql_mcm_nw.h"
#include "ql_mcm_dm.h"
#include "DSI_ConnectManager.h"

extern void goWanStatusCb(int status);
*/
import "C"

import (
	"fmt"
	"os"
	"os/signal"
	"syscall"
	"time"
	"unsafe"
)

// This APN profile is fixed for this deployment (Airtel IoT, profile 1,
// IPv4, PAP auth - confirmed via example_apn_v2 on the target module).
const (
	apn          = "airteliot.com"
	apnUser      = ""
	apnPass      = ""
	profileIndex = 1
)

func logf(format string, args ...interface{}) {
	fmt.Fprintf(os.Stdout, "[ql_autoconnect] "+format+"\n", args...)
}

// retryUntil calls fn every interval until it returns true or timeout elapses.
// Returns true if fn succeeded.
func retryUntil(name string, timeout, interval time.Duration, fn func() bool) bool {
	deadline := time.Now().Add(timeout)
	for {
		if fn() {
			return true
		}
		if time.Now().After(deadline) {
			logf("%s: timed out after %s", name, timeout)
			return false
		}
		time.Sleep(interval)
	}
}

func ensureAirplaneModeOff() error {
	var hDm C.dm_client_handle_type
	if ret := C.QL_MCM_DM_Client_Init(&hDm); ret != C.E_QL_SUCCESS {
		return fmt.Errorf("QL_MCM_DM_Client_Init failed: %d", ret)
	}
	defer C.QL_MCM_DM_Client_Deinit(hDm)

	var mode C.E_QL_MCM_DM_AIRPLANE_MODE_TYPE_T
	if ret := C.QL_MCM_DM_GetAirplaneMode(hDm, &mode); ret != C.E_QL_SUCCESS {
		return fmt.Errorf("QL_MCM_DM_GetAirplaneMode failed: %d", ret)
	}

	if mode == C.E_QL_MCM_DM_AIRPLANE_MODE_ON {
		logf("airplane mode is ON, switching it OFF")
		if ret := C.QL_MCM_DM_SetAirplaneMode(hDm, C.E_QL_MCM_DM_AIRPLANE_MODE_OFF); ret != C.E_QL_SUCCESS {
			return fmt.Errorf("QL_MCM_DM_SetAirplaneMode failed: %d", ret)
		}
	}
	return nil
}

// waitForSimReady blocks until the SIM is present and its 3GPP application is
// ready. This SIM doesn't use a PIN, so PIN entry isn't handled here.
func waitForSimReady(timeout time.Duration) bool {
	var hSim C.sim_client_handle_type
	if ret := C.QL_MCM_SIM_Client_Init(&hSim); ret != C.int(C.E_QL_SUCCESS) {
		logf("QL_MCM_SIM_Client_Init failed: %d", ret)
		return false
	}
	defer C.QL_MCM_SIM_Client_Deinit(hSim)

	return retryUntil("waitForSimReady", timeout, 2*time.Second, func() bool {
		var status C.QL_MCM_SIM_CARD_STATUS_INFO_T
		if ret := C.QL_MCM_SIM_GetCardStatus(hSim, C.E_QL_MCM_SIM_SLOT_ID_1, &status); ret != C.E_QL_SUCCESS {
			logf("QL_MCM_SIM_GetCardStatus failed: %d", ret)
			return false
		}

		if status.e_card_state != C.E_QL_MCM_SIM_CARD_STATE_PRESENT {
			logf("SIM card state: %d (waiting for PRESENT)", status.e_card_state)
			return false
		}

		app := status.card_app_info.app_3gpp
		if app.app_state == C.E_QL_MCM_SIM_APP_STATE_READY {
			logf("SIM ready")
			return true
		}
		logf("SIM app state: %d (waiting for READY)", app.app_state)
		return false
	})
}

// waitForNetworkRegistered blocks (via polling) until the modem reports full
// data-domain network registration.
func waitForNetworkRegistered(timeout time.Duration) bool {
	var hNw C.nw_client_handle_type
	if ret := C.QL_MCM_NW_Client_Init(&hNw); ret != C.E_QL_SUCCESS {
		logf("QL_MCM_NW_Client_Init failed: %d", ret)
		return false
	}
	defer C.QL_MCM_NW_Client_Deinit(hNw)

	return retryUntil("waitForNetworkRegistered", timeout, 2*time.Second, func() bool {
		var reg C.QL_MCM_NW_REG_STATUS_INFO_T
		if ret := C.QL_MCM_NW_GetRegStatus(hNw, &reg); ret != C.E_QL_SUCCESS {
			logf("QL_MCM_NW_GetRegStatus failed: %d", ret)
			return false
		}
		if reg.data_registration_valid == 0 {
			logf("data registration status not yet available")
			return false
		}
		state := reg.data_registration.registration_state
		logf("data registration state: %d", state)
		return state == C.E_QL_MCM_NW_SERVICE_FULL
	})
}

var wanEventCh = make(chan int, 16)

//export goWanStatusCb
func goWanStatusCb(status C.int) {
	select {
	case wanEventCh <- int(status):
	default:
	}
}

func wanEventName(ev int) string {
	switch C.QL_DSI_NET_EVT_T(ev) {
	case C.QL_DSI_EVT_NET_IS_CONN:
		return "NET_IS_CONN"
	case C.QL_DSI_EVT_NET_NO_NET:
		return "NET_NO_NET"
	case C.QL_DSI_EVT_NET_NEWADDR:
		return "NET_NEWADDR"
	case C.QL_DSI_EVT_NET_DELADDR:
		return "NET_DELADDR"
	case C.QL_DSI_EVT_WDS_CONNECTED:
		return "WDS_CONNECTED"
	case C.QL_DSI_EVT_PHYSLINK_UP_STATE:
		return "PHYSLINK_UP"
	case C.QL_DSI_EVT_PHYSLINK_DOWN_STATE:
		return "PHYSLINK_DOWN"
	default:
		return fmt.Sprintf("EVT_%d", ev)
	}
}

func startDataCall() bool {
	if ret := C.ql_wan_init(); ret != C.QL_WAN_SUCCESS {
		logf("ql_wan_init failed: %d", ret)
		return false
	}

	C.ql_wan_set_profile_index(profileIndex)
	C.ql_wan_set_pdp_type(C.int(C.QL_PDP_TYPE_IPV4_V01))
	C.ql_wan_set_ip_version(C.QL_DSI_IP_VERSION_4)

	apnC := C.CString(apn)
	userC := C.CString(apnUser)
	passC := C.CString(apnPass)
	defer C.free(unsafe.Pointer(apnC))
	defer C.free(unsafe.Pointer(userC))
	defer C.free(unsafe.Pointer(passC))
	if ret := C.ql_wan_setapn(apnC, userC, passC, C.QL_DSI_AUTH_PREF_ONLY_PAP); ret != C.QL_WAN_SUCCESS {
		logf("ql_wan_setapn failed: %d", ret)
	}

	// Let the library reconnect automatically if the link drops.
	C.ql_wan_set_autoconnect(1, 30)

	if ret := C.ql_wan_start((C.nw_status_cb)(C.goWanStatusCb)); ret != C.QL_WAN_SUCCESS {
		logf("ql_wan_start failed: %d", ret)
		return false
	}
	return true
}

func main() {
	logf("starting: profile=%d apn=%q", profileIndex, apn)

	if err := ensureAirplaneModeOff(); err != nil {
		logf("warning: %v (continuing anyway)", err)
	}

	if !waitForSimReady(60 * time.Second) {
		logf("giving up waiting for SIM; exiting so the init system can restart us")
		os.Exit(1)
	}

	if !waitForNetworkRegistered(120 * time.Second) {
		logf("giving up waiting for network registration; exiting so the init system can restart us")
		os.Exit(1)
	}

	if !startDataCall() {
		logf("giving up starting the data call; exiting so the init system can restart us")
		os.Exit(1)
	}

	sigCh := make(chan os.Signal, 1)
	signal.Notify(sigCh, syscall.SIGTERM, syscall.SIGINT)

	statsTicker := time.NewTicker(60 * time.Second)
	defer statsTicker.Stop()

	for {
		select {
		case ev := <-wanEventCh:
			logf("data call event: %s", wanEventName(ev))

		case <-statsTicker.C:
			var stats C.ql_dsi_data_pkt_stats
			if ret := C.ql_wan_get_pkt_stats(&stats); ret == C.QL_WAN_SUCCESS {
				logf("stats: tx=%d rx=%d bytes_tx=%d bytes_rx=%d",
					uint64(stats.pkts_tx), uint64(stats.pkts_rx),
					int64(stats.bytes_tx), int64(stats.bytes_rx))
			}

		case <-sigCh:
			logf("shutting down")
			C.ql_wan_stop()
			C.ql_wan_deinit()
			return
		}
	}
}
