/*==========================================================================
 |							QUECTEL - Build a smart world.
 |
 |							Copyright(c) 2017 QUECTEL Incorporated.
 |
 |--------------------------------------------------------------------------
 | File Description
 | ----------------
 |			quectel raw data 
 |
 |--------------------------------------------------------------------------
 |
 |	Designed by 		: 	will.shao
 |--------------------------------------------------------------------------
 | Revision History
 | ----------------
 |	2018/05/04				will.shao 		 Initialization
 |	------------------------------------------------------------------------
 \=========================================================================*/


#ifndef _QUEC_RAW_FLASH_DATA_H
#define _QUEC_RAW_FLASH_DATA_H

#include <linux/quectel/quec_raw_data_item.h>

bool quec_raw_flash_data_open(void);
bool quec_raw_flash_data_close(void);

bool quec_raw_flash_data_write_item(quec_raw_data_id data_id, void* data);
bool quec_raw_flash_data_read_item(quec_raw_data_id data_id, void* data);
bool quec_raw_flash_data_sync(void);

#if 1/*@Gavin.lu 20231124 Add usb boot From Kernel,here Get usbcfg from rowdata */
int quec_rawdata_read_usb_cfg(struct rawdata_item *item_usb1);
#endif

/* Zeke 20250424 Add use usb full speed mode */
int quec_rawdata_read_usb_full_speed(void);

#endif //_QUEC_RAW_FLASH_DATA_H

