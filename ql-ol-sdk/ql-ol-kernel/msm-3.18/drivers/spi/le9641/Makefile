#
# Makefile for multiplexer spi chip drivers.

obj-$(CONFIG_SPI_LE9641)	+= spi-le9641.o

obj-y	                        += sys_service.o
obj-y	                        += vp_hal.o

ccflags-$(CONFIG_SPI_DEBUG) := -DDEBUG
