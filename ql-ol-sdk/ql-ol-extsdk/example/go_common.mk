#--------------------------------------------------------------
#               Quectel OpenLinux
#--------------------------------------------------------------
# Shared cross-compile settings for every Go daemon under
# ql-ol-extsdk/example/*_go. A daemon's own Makefile sets BIN (and
# optionally GO_EXTRA_LDLIBS / GOFLAGS for anything it uniquely needs)
# before including this file:
#
#   BIN := ql_whatever
#   GO_EXTRA_LDLIBS := -lsomevendorlib   # only if this daemon needs it
#   include ../go_common.mk
#
# Requires the toolchain env (ql-ol-crosstool-env-init) to already be
# sourced, same as every other example in this tree.
QL_SDK_PATH ?= $(shell pwd)/../..
EXTSDK_INC  := $(QL_SDK_PATH)/include
EXTSDK_LIB  := $(QL_SDK_PATH)/lib

export CGO_ENABLED := 1
export GOOS         = linux
export GOARCH       = arm
export GOARM        = 7

export CGO_CFLAGS  := --sysroot=$(SDKTARGETSYSROOT)               \
                       -I$(EXTSDK_INC)                             \
                       -I$(SDKTARGETSYSROOT)/usr/include            \
                       -I$(SDKTARGETSYSROOT)/usr/include/data        \
                       -I$(SDKTARGETSYSROOT)/usr/include/dsutils      \
                       -I$(SDKTARGETSYSROOT)/usr/include/qmi           \
                       -I$(SDKTARGETSYSROOT)/usr/include/qmi-framework

# Common link deps every daemon here needs (MCM SIM/NW/DM device-info
# APIs). Anything binary-specific (e.g. dsi_netctrl for a data call)
# goes in that daemon's own GO_EXTRA_LDLIBS, set before including this file.
export CGO_LDFLAGS := --sysroot=$(SDKTARGETSYSROOT)         \
                       -L$(SDKTARGETSYSROOT)/usr/lib          \
                       -L$(EXTSDK_LIB)                         \
                       -lql_common_api -lqmiservices -lqmi_cci  \
                       -lqmi_common_so -lqmi -lql_sys_log -lpthread \
                       $(GO_EXTRA_LDLIBS)

all: $(BIN)

$(BIN):
	go build -o $(BIN) .

clean:
	rm -f $(BIN)

.PHONY: all clean
