/**
 * @file ql_mgtm_usb.h
 * @brief Quectel USB interface function declarations.
 *
 * @note
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
  ------------     -------     ----------------------------------------
  11/13/2018       Mike        Initial creation.
============================================================================*/
#ifndef __QL_MGMT_USB_H__
#define __QL_MGMT_USB_H__

typedef enum {
	QL_MGMT_CLIENT_COMMAND_USB_CONFIG_WRITE             = 0x0001,
	QL_MGMT_CLIENT_COMMAND_USB_ACTION                   = 0x0002,
	QL_MGMT_CLIENT_COMMAND_USB_BOOTUP                   = 0x0003,
	QL_MGMT_CLIENT_COMMAND_USB_ID                       = 0x0004,
	QL_MGMT_CLIENT_COMMAND_USB_PRODUCT_INFO             = 0x0005,
	QL_MGMT_CLIENT_COMMAND_USB_MANUFAC_INFO             = 0x0006,
	QL_MGMT_CLIENT_COMMAND_USB_NET                      = 0x0007,	
	QL_MGMT_CLIENT_COMMAND_USB_FUNC                     = 0x0008,
	QL_MGMT_CLIENT_COMMAND_USB_CONFIG                   = 0x0009,
	QL_MGMT_CLIENT_COMMAND_USB_INTERFACE_WRITE          = 0x000A,
	QL_MGMT_CLIENT_COMMAND_USB_INTERFACE                = 0x000B,
} ql_mgmt_client_command_usb_e;

struct ql_mgmt_client_usb_action {
	usb_num_e_v01 num;
	usb_action_type_e_v01 action_type;
};

struct ql_mgmt_client_usb_bootup {
	usb_num_e_v01 num;
	usb_bootup_e_v01 up;
};

struct ql_mgmt_client_usb_id {
	usb_num_e_v01 num;
	unsigned short pid;
	unsigned short vid;
};

struct ql_mgmt_client_usb_product_info {
	usb_num_e_v01 num;
	char product_info[USB_PRODUCT_INFO_MAX_LEN_V01];
};

struct ql_mgmt_client_usb_manufac_info {
	usb_num_e_v01 num;
	char manufac_info[USB_MANUFAC_INFO_MAX_LEN_V01];
};

struct ql_mgmt_client_usb_net {
	usb_num_e_v01 num;
	usb_net_type_e_v01 net;
};

struct ql_mgmt_client_usb_func {
	usb_num_e_v01 num;
	bool diag;
	bool nmea;
	bool at;
	bool modem;
	bool net;
	bool adb;
	bool audio_uac;
};

struct ql_mgmt_client_usb_config {
	usb_num_e_v01 num;
	usb_bootup_e_v01 up;
	unsigned short pid;
	unsigned short vid;
	char product_info[USB_PRODUCT_INFO_MAX_LEN_V01];
	char manufac_info[USB_MANUFAC_INFO_MAX_LEN_V01];
	usb_net_type_e_v01 usbnet;
	bool diag;
	bool nmea;
	bool at;
	bool modem;
	bool net;
	bool adb;
	bool audio_uac;
};

struct ql_mgmt_client_usb_interface {
	usb_num_e_v01 num;
	int usb_fixed_interface;
};

typedef struct {
	union {
		struct ql_mgmt_client_usb_action action;
		struct ql_mgmt_client_usb_bootup up;
		struct ql_mgmt_client_usb_id id;
		struct ql_mgmt_client_usb_product_info product_info;
		struct ql_mgmt_client_usb_manufac_info manufac_info;
		struct ql_mgmt_client_usb_net net;
		struct ql_mgmt_client_usb_func func;
		struct ql_mgmt_client_usb_config config;
		struct ql_mgmt_client_usb_interface interface;
	};
} ql_mgmt_client_usb_s;

#endif /* end of __QL_MGMT_USB_H__ */
