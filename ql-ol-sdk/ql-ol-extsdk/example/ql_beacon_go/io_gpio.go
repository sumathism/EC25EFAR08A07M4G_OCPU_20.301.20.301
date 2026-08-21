package main

/*
#cgo CFLAGS: -march=armv7-a -mfloat-abi=softfp -mfpu=neon
#include <ql_oe.h>
#include <ql_gpio.h>
#include <ql_adc.h>
*/
import "C"

import (
	"ql_beacon/beacon/modules"
)

// Real hardware wired to each channel Channels with no real equivalent on this hardware are absent from these maps, not stubbed with a fake pin - see io.go's comments for why each one was excluded.
var diPins = map[int]C.Enum_PinName{
	modules.DI0: C.PINNAME_GPIO1,
	modules.DI1: C.PINNAME_GPIO2,
	modules.DI2: C.PINNAME_GPIO3,
}

var doPins = map[int]C.Enum_PinName{
	modules.DO0: C.PINNAME_GPIO4,
	modules.DO1: C.PINNAME_GPIO5,
}

var aiChannels = map[int]C.Enum_QADC{
	modules.AI0:  C.ADC0,
	modules.AI1:  C.ADC1,
	modules.AI2:  C.ADC2,
	modules.VBAT: C.CBC,
}

var ioInputsInitialized bool

// initIOInputsOnce configures every real digital pin - inputs as inputs,
// outputs as outputs (starting low, since nothing commands them yet)-once. ADC/CBC channels need no equivalent setup.
func initIOInputsOnce() {
	if ioInputsInitialized {
		return
	}
	for _, pin := range diPins {
		if ret := C.Ql_GPIO_Init(pin, C.PINDIRECTION_IN, C.PINLEVEL_LOW, C.PINPULLSEL_DISABLE); ret != 0 {
			logf("Ql_GPIO_Init(pin=%d, in) failed: %d", pin, ret)
		}
	}
	for _, pin := range doPins {
		if ret := C.Ql_GPIO_Init(pin, C.PINDIRECTION_OUT, C.PINLEVEL_LOW, C.PINPULLSEL_DISABLE); ret != 0 {
			logf("Ql_GPIO_Init(pin=%d, out) failed: %d", pin, ret)
		}
	}
	ioInputsInitialized = true
}

// getIOInfo reads every real digital input, digital output, and analog channel fresh, right now, and packs the results into IO_DATA_V2_29's shape at their spec-defined bit positions.
func getIOInfo() modules.IOInfo {
	initIOInputsOnce()

	var info modules.IOInfo

	for bit, pin := range diPins {
		info.DIEnabled |= 1 << uint(bit)
		lvl := C.Ql_GPIO_GetLevel(pin)
		if lvl < 0 {
			logf("Ql_GPIO_GetLevel(pin=%d) failed: %d", pin, lvl)
			continue
		}
		if lvl != 0 {
			info.DIState |= 1 << uint(bit)
		}
	}

	for bit, pin := range doPins {
		info.DOEnabled |= 1 << uint(bit)
		lvl := C.Ql_GPIO_GetLevel(pin) // reads back what this output is currently driving
		if lvl < 0 {
			logf("Ql_GPIO_GetLevel(pin=%d) failed: %d", pin, lvl)
			continue
		}
		if lvl != 0 {
			info.DOState |= 1 << uint(bit)
		}
	}

	for bit, channel := range aiChannels {
		mv := C.ql_adc_show(channel)
		if mv < 0 {
			logf("ql_adc_show(channel=%d) failed: %d", channel, mv)
			continue
		}
		info.AIEnabled |= 1 << uint(bit)
		info.AIValues[bit] = int32(mv)
	}

	return info
}
