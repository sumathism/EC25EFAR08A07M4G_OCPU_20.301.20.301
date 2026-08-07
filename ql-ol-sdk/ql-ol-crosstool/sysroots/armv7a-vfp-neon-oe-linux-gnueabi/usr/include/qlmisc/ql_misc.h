#ifndef __QL_MISC_H
#define __QL_MISC_H
#include <stdio.h>
#include <stdlib.h>
//add by dawn 2018-9-04

/* add by [jared.lu] 20180515 for sysconfig written to misc partition*/
#define QUECTEL_SYSCONFIG_MISC
/*MISC flag Structure*/
#define quec_pr(format, ...) fprintf(stderr, "[%s  %d]: "format, __func__, __LINE__, ##__VA_ARGS__)
typedef enum{
	SYS_CONFIG_START=0x7FFFFF00,
    SYS_CONFIG_AP_RSTLEVEL_ID,     
    SYS_CONFIG_MODEM_RSTLEVEL_ID, 
    SYS_CONFIG_END=0x7FFFFF20 //max flags 31     
}Enum_SysCfgId;

typedef struct{
	int  enable;		//normally 1 or 0
	char command[32];
	char status[32];
	char string[1024]; 
}ST_Datacfg;

typedef struct{
    Enum_SysCfgId   itemId;   // Data item ID
    unsigned int    size;   // Size of one record
    ST_Datacfg      Value;
}ST_SystemConfig;

/* for add sysconfig commandline*/
//int get_sysconfig_from_misc(Enum_SysCfgId syscfgid, ST_SystemConfig *systemcfg);
//int set_sysconfig_to_misc(Enum_SysCfgId syscfgid, ST_SystemConfig *systemcfg);

/* MISC partition. */
int mtd_open_partition_name(const char *partition_name, unsigned int *dev_id,
                                   unsigned int *size, unsigned int *erasesize);
static unsigned int ql_mtd_write_data(int fd, unsigned int  offset, const unsigned char *data,
                                   unsigned int data_len, unsigned int erasesize);
unsigned int mtd_get_page_size(unsigned int dev_id);
int mtd_check_bad_block(int fd, unsigned int  offset);
static  unsigned int ql_mtd_read_data(int fd, unsigned int  offset, const unsigned char *data,unsigned int data_len);
int quectel_write_misc(Enum_SysCfgId param_id, ST_SystemConfig systemcfg);
int quectel_read_misc(Enum_SysCfgId param_id, ST_SystemConfig *systemcfg);
#endif
