/**
 * @file usb_product_config.h
 * @brief Quectel USB Product configuration.
 * 
 * @note Provide Linux usb product information functions
 */
/*=============================================================================
  Copyright (c) 2018 Quectel Wireless Solution, Co., Ltd.  All Rights Reserved.
  Quectel Wireless Solution Proprietary and Confidential.
=============================================================================*/

/*============================================================================

                          EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  WHEN             WHO         WHAT, WHERE, WHY
  ------------     -------     -----------------------------------------------
  11/08/2018       Mike        Initial creation.
  01/11/2019       Mike        Modifed usb product description.
==============================================================================*/
#ifndef __USB_PRODUCT_CONFIG_H__
#define __USB_PRODUCT_CONFIG_H__

enum {
	USB_CFG_USBNET_TYPE_RMNET = 0,
	USB_CFG_USBNET_TYPE_ECM,
	USB_CFG_USBNET_TYPE_MBIM,
	USB_CFG_USBNET_TYPE_RNDIS
} usb_cfg_net_type_e;

enum {
	USB_BOOTUP_FS_UP = 0,
	USB_BOOTUP_MODEM_UP,
	USB_BOOTUP_LINUX_UP
} usb_bootup_type_e;

#ifndef USB_BOOTUP_DEFAULT_SET
#define USB_BOOTUP_DEFAULT_SET USB_BOOTUP_MODEM_UP
#endif

#ifndef USB_PID
#define USB_PID 0x0125
#endif

#ifndef USB_VID
#define USB_VID 0x2C7C
#endif

#ifndef USB_MANUFAC
#define USB_MANUFAC "Android"
#endif

#ifndef USB_PRODUCT
#define USB_PRODUCT "Android"
#endif

#ifndef DIAG_DEFAULT_SET
#define DIAG_DEFAULT_SET (1)
#endif

#ifndef NMEA_DEFAULT_SET
#define NMEA_DEFAULT_SET (1)
#endif

#ifndef AT_DEFAULT_SET
#define AT_DEFAULT_SET (1)
#endif

#ifndef MODEM_DEFAULT_SET
#define MODEM_DEFAULT_SET (1)
#endif

#ifndef NET_DEFAULT_SET
#define NET_DEFAULT_SET (1)
#endif

#ifndef ADB_DEFAULT_SET
#ifdef QUECTEL_FEATURE_OPENLINUX
#define ADB_DEFAULT_SET (1)
#else
#define ADB_DEFAULT_SET (0)
#endif
#endif

#ifndef AUDIO_UAC_DEFAULT_SET
#define AUDIO_UAC_DEFAULT_SET (0)
#endif

#ifndef USBNET_DEFAULT_SET
#define USBNET_DEFAULT_SET USB_CFG_USBNET_TYPE_RMNET
#endif

#endif /* __USB_PRODUCT_CONFIG_H__ */
