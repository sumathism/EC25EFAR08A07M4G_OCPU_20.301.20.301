
/*==========================================================================
 |                     QUECTEL - Build a smart world.
 |
 |                Copyright(c) 2017 QUECTEL Incorporated.
 |
 |--------------------------------------------------------------------------
 | File Description
 | ----------------
 | quectel raw data item
 |
 |--------------------------------------------------------------------------
 |
 | Designed by: will.shao
 |--------------------------------------------------------------------------
 | Revision History
 | ----------------
 | 2018/05/04               will.shao            Initialization
 | 2018/11/29               Mike                 Modify usb struct.
 | 2019/02/02               Mike                 Adds download infomation type.
 | 2020/02/18               Charles      		 Adds softsim features
 | 2020/04/29               colin                add bt spi control
 |------------------------------------------------------------------------
 \=========================================================================*/

#ifndef _QUEC_RAW_DATA_ITEM_H_
#define _QUEC_RAW_DATA_ITEM_H_

#include <linux/types.h>

typedef __u8 byte;
typedef __u32 uint32_t;

#define QUEC_PACKED
#define QUEC_PACKED_POST __attribute__((__packed__))

#define RAW_FLASH_DATA_BLOCK_NUMS (5)
#define QUECTEL_RAW_DATA_PARTITION_NAME ("rawdata")

#define QUEC_IOCTL_RAWDATA_GET_ITEM 		(0x00050001)
#define QUEC_IOCTL_RAWDATA_SET_ITEM 		(0x00050002)

#define RAW_DATA_ITEM_MAX_LEN (128)
#define RAW_DATA_ITEM_NUMS (30)

typedef QUEC_PACKED struct
{
	int 	data_len;
	byte	data[RAW_DATA_ITEM_MAX_LEN];
} QUEC_PACKED_POST raw_data_type;

QUEC_PACKED struct rawdata_item {
	int id;
	raw_data_type value;
} QUEC_PACKED_POST;

// RAW DATA ID
typedef enum
{
	RAW_DATA_SYSTEM_CFG = 0,
	RAW_DATA_DOWNLOAD_INFO_ID,
	RAW_DATA_SOFTSIM_CFG,
	RAW_DATA_BT_FLAG_CFG,
	RAW_DATA_FLAG_CTL_CFG, //<2020/11/20>add colin quectel rawdata flag manage 
    RAM_DATA_FIRMWARE_VERSION,
	RAW_DATA_SECURE_CFG,
	RAW_DATA_MAX_ID,  
} quec_raw_data_id;

#define QUEC_RAW_SYSTEM_MAGIC               0X20181210
#define QUEC_RAW_SOFTSIM_MAGIC 				0x20200218
#define QUEC_RAW_BT_FLAG_MAGIC              0x20200429
//<2020/11/20>add colin quectel rawdata flag manage 
#define QUEC_RAW_FLAG_CTL_MAGIC             0x20201120  

typedef QUEC_PACKED struct
{
	int magic;
	unsigned short vid;
	unsigned short pid;
	char usb_manuf_info[32];
	char usb_product_info[32];
	unsigned char bootup;
	unsigned char usbnet;
	unsigned int mask;
	unsigned char usb_diag_flag;
	unsigned char usb_fixed_interface;
	char resv[42];
	unsigned char wifi_mac[6];
} QUEC_PACKED_POST ql_system_cfg_type;

typedef enum
{
	BOOT_MODE_DOWNLOAD,
	BOOT_MODE_BACKUP_SBL,
	BOOT_MODE_NORMAL,
} boot_mode_type;

typedef QUEC_PACKED struct 
{
	uint32_t boot_mode;
	uint32_t port;
	uint32_t baudrate;
	uint32_t fota;
} QUEC_PACKED_POST quec_download_info_type;

typedef QUEC_PACKED struct
{
	int magic;
	unsigned char enable;
	unsigned char download_flag;
	char resv[122];
} QUEC_PACKED_POST ql_softsim_cfg_type;

/*add by colin.cui 2020.01.06 bt spi control function.*/
//#ifdef QUECTEL_AT_BTSPI_SET_SUPPORT
typedef QUEC_PACKED struct
{
	unsigned int bt_magic;
	unsigned int bt_flag;
} QUEC_PACKED_POST ql_bt_cfg_type;
//#endif

//<2020/11/20>add colin quectel rawdata flag manage --start
#define RAWDATA_MANAGE_LEN 64

typedef QUEC_PACKED struct
{
	unsigned int flagctl_magic;
	unsigned int used_total_len;
	char flag_sum;
	char data[RAWDATA_MANAGE_LEN];
} QUEC_PACKED_POST ql_rawdata_flag_ctl_type;
//<2020/11/20>add colin quectel rawdata flag manage --end

#define QUEC_RAW_FIRMARE_MAGIC1     0x20190424
#define QUEC_RAW_FIRMARE_MAGIC2     0x20190425

#define RAWDATA_VERSION         "V1.00"

typedef struct {
    unsigned int magic1;
    unsigned int magic2;
    char rawdata_version[8];
    char project_name[32];
    char project_type[8];
    char reserve[72];
} QUEC_PACKED_POST ql_firmware_version_type;

typedef struct {
    char pin[44];
	unsigned short pwd_status;
	unsigned short dbg_status;
	char reserve[80];
} QUEC_PACKED_POST ql_secure_cfg_type;

#define QUECTEL_SECURE_DEBUG_FEATURE

#endif //_QUEC_RAW_DATA_ITEM_H_
