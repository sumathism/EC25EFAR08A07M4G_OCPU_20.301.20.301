/**
 * @file quectel_file.c
 * @brief Quectel file public function declarations.
 *
 * @note
 *
 * @copyright Copyright (c) 2010-2017 @ Quectel Wireless Solutions Co., Ltd.
 */
/*=============================================================================
   Copyright (c) 2018 Quectel Wireless Solution, Co., Ltd.  All Rights Reserved.
   Quectel Wireless Solution Proprietary and Confidential.
=============================================================================*/
 
/*============================================================================
 
			   EDIT HISTORY FOR MODULE
   
   This section contains comments describing changes made to the module.
   Notice that changes are listed in reverse chronological order.
   
   WHEN 	    WHO 	WHAT, WHERE, WHY
   ------------     -------	----------------------------------------
   11/08/2018	    Mike	Extern quectel_file_write_int function.
   20/03/2023       Harry       add write/read bt.inf file function.
============================================================================*/ 
#ifndef __QUECTEL_FILE_H__
#define __QUECTEL_FILE_H__

/**
 * writes the buffer pointed buf to the file
 * 
 * @param[in] file           file absolute path
 * @param[in] buf            the buffer pointed
 *
 * @return 
 *   On success, 0 is returned.  On error, -1 is returned.
 * 
 */
extern int quectel_file_write_string(const char *file, const char *buf);

/**
 * writes the int buf to the file
 * 
 * @param[in] file           file absolute path
 * @param[in] buf            the buffer pointed
 *
 * @return 
 *   On success, 0 is returned.  On error, -1 is returned.
 * 
 */
extern int quectel_file_write_int(const char *file, const int base, const int buf);

/**
 * writes WiFi Mac address to wlan_mac.bin file
 * 
 * @param[in] mac            The WiFi Mac address, 6 bytes
 *
 * @return 
 *   On success, 0 is returned.  On error, -1 is returned.
 * 
 */
extern int quectel_wlan_mac_bin_write(char *mac);

/**
 * read WiFi Mac address from wlan_mac.bin file
 * 
 * @param[out] mac            The WiFi Mac address, 6 bytes
 *
 * @return 
 *   On success, 0 is returned.  On error, -1 is returned.
 * 
 */
extern int quectel_wlan_mac_bin_read(char *mac);

/**
 * read active WiFi Mac address from /sys/class/net/wlanX/address file
 * 
 * @param[out] mac            The WiFi Mac address, 6 bytes
 *
 * @param[out] number         Number of WLAN currently enable
 *
 * @return 
 *   On success, 0 is returned. On error, -1 is returned.
 * 
 */
extern int quectel_active_wlan_mac_read(char *mac, const int number);

/**
 * writes BT Mac address to bt.inf file
 * 
 * @param[in] mac            The BT Mac address, 6 bytes
 *
 * @return 
 *   On success, 0 is returned.  On error, -1 is returned.
 * 
 */
extern int quectel_bt_mac_inf_write(char *mac);

/**
 * read BT Mac address from bt.inf file
 * 
 * @param[out] mac            The BT Mac address, 6 bytes
 *
 * @return 
 *   On success, 0 is returned.  On error, -1 is returned.
 * 
 */
extern int quectel_bt_mac_inf_read(char *mac);

/**
 * read a line of data from the file
 * 
 * @param[in] file_name            file path
 * @param[in] line  		       read the number of line in the file
 * @param[out] buf  		       buffer
 * @param[in] len  		       	   buffer length
 *
 * @return 
 *   On success, 0 is returned.  On error, -1 is returned.
 * 
 */
char* ql_get_line_from_file(const char* file_name, int line, char* buf, int len);

#endif /* end of __QUECTEL_FILE_H__ */
