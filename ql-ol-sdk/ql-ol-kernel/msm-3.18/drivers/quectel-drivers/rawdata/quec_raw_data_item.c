/*==========================================================================
 |                    QUECTEL - Build a smart world.
 |
 |               Copyright(c) 2017 QUECTEL Incorporated.
 |
 |--------------------------------------------------------------------------
 | File Description
 | ----------------
 | quectel crc 
 |
 |--------------------------------------------------------------------------
 |
 | Designed by: will.shao
 |--------------------------------------------------------------------------
 | Revision History
 | ----------------
 | 2018/05/04               will.shao               Initialization
 | 2018/11/18               Mike                    Modify data type
 | 2019/02/02               Mike                    Adds download infomation type.
 | 2020/02/18               Charles     			 	Adds softsim features
 | 2020/04/29               colin                   add bt spi control
 | ------------------------------------------------------------------------
 \=========================================================================*/
#include <linux/quectel/quec_raw_data_item.h>

const uint32_t quec_raw_data_item_len_list[RAW_DATA_MAX_ID] =
{
	sizeof(ql_system_cfg_type),                  //RAW_DATA_SYSTEM_CFG
	sizeof(quec_download_info_type),             //RAW_DATA_DOWNLOAD_INFO_ID
	sizeof(ql_softsim_cfg_type),               //RAW_DATA_SOFTSIM_CFG
	sizeof(ql_bt_cfg_type),                  //RAW_DATA_BT_FLAG_CFG  #ifdef QUECTEL_AT_BTSPI_SET_SUPPORT
	sizeof(ql_rawdata_flag_ctl_type),
    sizeof(ql_firmware_version_type),               //RAM_DATA_FIRMWARE_VERSION
	sizeof(ql_secure_cfg_type),				//RAW_DATA_SECURE_CFG
};
