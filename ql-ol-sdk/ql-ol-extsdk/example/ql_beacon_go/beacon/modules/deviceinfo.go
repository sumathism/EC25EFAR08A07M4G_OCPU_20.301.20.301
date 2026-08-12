// Package modules holds per-beacon-module payload builders. Each one only
// knows how to encode its own TLV payload - framing, CRC, encryption and
// transport all live in the beacon package and don't change as modules are
// added here.
package modules

// DeviceInfo key codes, matching DEVICE_INFO_6 in the LAFV2 firmware
// (lafm_beacon.h) exactly, so the server's existing parser recognizes them.
const (
	KeyDeviceID    = 1
	KeyFWName      = 2
	KeyIMEI        = 3
	KeySIMNo       = 4
	KeyPubKey      = 11
	KeyNetOperator = 13
	KeyCommModule  = 14
	KeyAPN         = 16
)

// DeviceInfo is the basic device identity/status info sent as module 6
// (DEVICE_INFO_6) of a type-100 beacon.
type DeviceInfo struct {
	DeviceID    string // reused from IMEI on this platform (no hardware serial register)
	FWName      string
	IMEI        string
	SIMNo       string // ICCID
	PubKey      []byte // this device's raw 32-byte X25519 public key - NOT hex-encoded;
	NetOperator string // the server/debug page hex-encodes binary fields for display itself
	CommModule  string // modem firmware version
	APN         string
}

// Encode builds the DEVICE_INFO_6 payload: nDevInfo(1B) + repeated
// [key(1B) + len(1B) + value(len bytes)]. Empty/unset fields are omitted
// rather than sent as zero-length entries.
func (d DeviceInfo) Encode() []byte {
	type entry struct {
		key   byte
		value []byte
	}
	var all []entry
	addString := func(key byte, s string) {
		if s != "" {
			all = append(all, entry{key, []byte(s)})
		}
	}

	addString(KeyDeviceID, d.DeviceID)
	addString(KeyFWName, d.FWName)
	addString(KeyIMEI, d.IMEI)
	addString(KeySIMNo, d.SIMNo)
	if len(d.PubKey) > 0 {
		all = append(all, entry{KeyPubKey, d.PubKey})
	}
	addString(KeyNetOperator, d.NetOperator)
	addString(KeyCommModule, d.CommModule)
	addString(KeyAPN, d.APN)

	payload := []byte{byte(len(all))}
	for _, e := range all {
		v := e.value
		if len(v) > 255 {
			v = v[:255]
		}
		payload = append(payload, e.key, byte(len(v)))
		payload = append(payload, v...)
	}
	return payload
}
