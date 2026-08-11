package modules

import "encoding/binary"

// Timestamp is TIMESTAMP_4: uptime_ms(u64) + utctime_ms(u64), both little
// endian - confirmed against lafm_beacon_create_100_add_timestamp_4 /
// lafm_app_beacon_server_utils_add_100_timestamp_4 in the firmware. LAFV2
// sends this as the first module of nearly every beacon it builds; it's
// what feeds the dashboard's utctime/gpstime pair (GNSS_INFO_38's own
// last_updated/last_fix fields are separate and feed gnsslastupdate/
// lastvalidfix instead).
//
// UTCTimeMs must come from the same single device clock used everywhere
// else (see gnss_callback.go's doc comment) - lafm_timer_get_epoch() is
// milliseconds (lafm_timer_get_epoch_s() = lafm_timer_get_epoch()/1000),
// so this is epoch milliseconds, not seconds.
type Timestamp struct {
	UptimeMs  uint64
	UTCTimeMs uint64
}

func (t Timestamp) Encode() []byte {
	buf := make([]byte, 16)
	binary.LittleEndian.PutUint64(buf[0:8], t.UptimeMs)
	binary.LittleEndian.PutUint64(buf[8:16], t.UTCTimeMs)
	return buf
}
