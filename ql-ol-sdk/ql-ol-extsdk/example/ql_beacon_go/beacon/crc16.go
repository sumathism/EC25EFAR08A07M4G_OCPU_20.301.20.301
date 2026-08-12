package beacon

// crc16 is the Nordic nRF5 SDK crc16_compute() variant used by the LA5 beacon
// protocol: CRC-16/CCITT, init 0xFFFF, no reflection, no final XOR. Ported
// line-for-line from lafm_beacon.c's dependency on the nRF5 SDK crc16 module.
func crc16(data []byte) uint16 {
	var crc uint16 = 0xFFFF
	for _, b := range data {
		crc = (crc >> 8) | (crc << 8)
		crc ^= uint16(b)
		crc ^= (crc & 0xff) >> 4
		crc ^= (crc << 8) << 4
		crc ^= ((crc & 0xff) << 4) << 1
	}
	return crc
}
