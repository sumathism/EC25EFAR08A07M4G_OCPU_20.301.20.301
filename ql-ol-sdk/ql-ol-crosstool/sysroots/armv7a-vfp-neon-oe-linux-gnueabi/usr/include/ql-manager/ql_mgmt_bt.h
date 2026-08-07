/**
 * @file ql_mgtm_bt.h
 * @brief Quectel BT interface function declarations.
 *
 * @note
 */
/*=============================================================================
  Copyright (c) 2023 Quectel Wireless Solution, Co., Ltd.  All Rights Reserved.
  Quectel Wireless Solution Proprietary and Confidential.
=============================================================================*/

/*============================================================================

                          EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  WHEN             WHO         WHAT, WHERE, WHY
  ------------     -------     ----------------------------------------
  03/21/2023       Harry        Initial creation.
============================================================================*/
#ifndef __QL_MGMT_BT_H__
#define __QL_MGMT_BT_H__

typedef enum {
	QL_MGMT_CLIENT_COMMAND_BT_MAC                               = 0x0001,
} ql_mgmt_client_command_bt_e;

struct ql_mgmt_client_bt_mac {
	char mac[6];
};

typedef struct {
	union {
		struct ql_mgmt_client_bt_mac mac;
	};
} ql_mgmt_client_bt_s;

#endif /* end of __QL_MGMT_BT_H__ */