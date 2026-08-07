/**
 * @file ql_bt.h
 * @brief Quectel Openlinux WiFi public function declarations.
 * 
 * @note 
 *
 * @copyright Copyright (c) 2009-2023 @ Quectel Wireless Solutions Co., Ltd.
 */

#ifndef __QL_BT_H__
#define __QL_BT_H__

#include <stdbool.h>
#include <netinet/in.h>

/**
 * Get the local BT address: compatible with FC20/FC21.
 *
 * @param [out]   char * bt_mac
 *
 * @return 
 *   On success, 0 is returned.  On error, -1 is returned.
 * 
 */
extern int ql_bt_mac_get(char * bt_mac);

#endif /* end of __QL_BT_H__ */