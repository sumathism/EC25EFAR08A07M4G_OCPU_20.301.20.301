#--------------------------------------------------------------
#               Quectel OpenLinux
#--------------------------------------------------------------
# Cross-compiles the ql_autoconnect Go daemon against the vendor
# MCM/QMI/dsi_netctrl libraries using cgo.
BIN              := ql_autoconnect
GO_EXTRA_LDLIBS  := -ldsi_netctrl -ldsutils

include ../go_common.mk
