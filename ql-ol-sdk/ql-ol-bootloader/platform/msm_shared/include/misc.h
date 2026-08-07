#ifndef __MISC_H
#define __MISC_H
/*MISC flag Structure*/
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
int get_sysconfig_from_misc(Enum_SysCfgId syscfgid, ST_SystemConfig *systemcfg);
int set_sysconfig_to_misc(Enum_SysCfgId syscfgid, ST_SystemConfig *systemcfg);
#endif
