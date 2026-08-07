/*!	@file
	qstart.c
	@brief
	This file provides apis to set restore flag to mtd nand
*/
/*===========================================================================
	Copyright(c) 2018 Quectel Wierless Solution,Co.,Ltd. All Rights Reserved.
	Quectel Wireless Solution Proprietary and Confidential.
============================================================================*/
/*===========================================================================
							EDIT HISTORY FOR MODULE
This section contains comments describing changees made to the module.
Notice that changes are listed in reverse chronological order.
WHEN		 WHO		WHAT,WHERE,WHY
----------	-----		----------------------------------------------------
06/24/2018	Ramos		add set efs restore flag in linux 
============================================================================*/

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/miscdevice.h>
#include <linux/workqueue.h>
#include <linux/uaccess.h>
#include <linux/mtd/mtd.h>
#include <linux/slab.h>
#include <linux/reboot.h>
#include <linux/qstart.h>

#include <linux/platform_device.h>
#include <soc/qcom/smem.h>
#include "quectel-features-config.h" // quectel add

#if 1 // def  QUECTEL_SYSTEM_BACKUP    // Ramos add for quectel for linuxfs restore

#define CEFS_FILE_MAGIC1        (0x51D24368)
#define CEFS_FILE_MAGIC2        (0x4378AC6E)

#ifdef QUECTEL_RUSSIAN_FORBIDDEN_FW_ROLLBACK_FEATURE
#define NEW_CEFS_FILE_MAGIC1        (0x52D24368)
#define NEW_CEFS_FILE_MAGIC2        (0x4378AC6E)
#endif

//add by francis , 20180613,add data magic for linux
#define QUEC_DATA_MAGIC1        (0x5F3759DF)
#define QUEC_DATA_MAGIC2        (0x43A2E930)

#define QUEC_BACKUP_MAGIC1        (0x78E5D4C2)
#define QUEC_BACKUP_MAGIC2        (0x54F7D60E)
#define QUEC_BACKUP_INFO_BLOCK_NUMS (3)  // keep with modem , bootloader aboot.c
//"sys_rev" partiton last QUEC_BACKUP_INFO_BLOCK_NUMS block use record restore information

#define QUEC_ALL_RESTORE_FLAG_BLOCK_INDEX (6) // the 3 block reserved for All parition restoring flag and fota upgraded flag
//the "sys_rev" partition last QUEC_ALL_RESTORE_FLAG_BLOCK_INDEX to QUEC_BACKUP_INFO_BLOCK_NUMS reserved for fota upgraded flag.

#define QUEC_START_TIMER_TOUT	30

#define BACKUP_INFO_BLOCK_NUMS (3)

//chongyu 2019-9-3 sys_rev partition get adaptive name
#if defined (CONFIG_PROJECT_EC21AUX_GA) || defined (CONFIG_PROJECT_EC25EM_GB) || defined (CONFIG_PROJECT_EC25AUX_GA) || defined (CONFIG_PROJECT_EC25AUX_GAA) || defined (CONFIG_PROJECT_EC21AUX_GAD) || defined (CONFIG_PROJECT_EG91EX_REL) \
|| defined (CONFIG_PROJECT_EC25MX) || defined (CONFIG_PROJECT_EG91EX) || defined (CONFIG_PROJECT_EG91_JP) || defined (CONFIG_PROJECT_EG95_JP) || defined (CONFIG_PROJECT_EG91EX_GAA) || defined (CONFIG_PROJECT_EG95EX) || defined (CONFIG_PROJECT_EC25EUX) || defined (CONFIG_PROJECT_EC21EUX) \
|| defined (CONFIG_PROJECT_EG95AUX) || defined (CONFIG_PROJECT_EG95AUX_DP)|| defined (CONFIG_PROJECT_EG91AUX) || defined (CONFIG_PROJECT_EC25EM_GC) || defined (CONFIG_PROJECT_EG95AUX_GAD) || defined (CONFIG_PROJECT_EG91AUX_GAD) || defined (CONFIG_PROJECT_EG95EX_GAA)
    #define SYS_REV_PARTITION_NAME "cache_efs2"
#else
    #define SYS_REV_PARTITION_NAME "sys_rev"
#endif

typedef struct
{    
    char ql_mtd_name[12];
    char ql_mtd_restore_name[12];
    uint64_t total_size;
    uint32_t ql_mtd_nub;
    uint32_t restore_flag;
    uint32_t restore_name;
    uint32_t backup_times;
    uint32_t crash[12];
} Ql_Mtd_Info;
//add by [francis.huan], 20180417, for partition info who need to restore or erase

typedef struct
{
  uint32_t magic1;  
  uint32_t magic2;

  uint32_t cefs_restore_flag;
  uint32_t cefs_restore_times;
  uint32_t cefs_backup_times;
  uint32_t cefs_crash[10];  // ÂŒ10Ø·cefsÄŒcrashÄŽ
   
  // Ramos add for linux fs backup restore times
    uint32_t linuxfs_restore_flag;
    uint32_t linuxfs_restore_times;
    uint32_t linuxfs_backup_times;  //ÎªLinux fsÇºÍŽÒ»ÂŒÄ£Ã»ÃµÊ¶Ç·LinuxÄŒÏµÍ³Ô­Ì¡Îª0xAA55 ò²»œÐ»Ô­
    uint32_t linuxfs_crash[10];  // ÂŒ10Ø·ÄŒÏµÍ³crashÄŽ

	// modem backup restore flag
	uint32_t modem_restore_flag;
	uint32_t modem_restore_times;
	uint32_t modem_backup_times;
	uint32_t modem_crash[10];  

    // other image restore flag
	uint32_t image_restoring_flag;
	uint32_t reserved1;
    uint32_t reserved2[100];

    uint32_t data_magic1;
    uint32_t data_magic2;
    
    Ql_Mtd_Info ql_mtd_info[6]; 
} quec_backup_info_type;

//modify by [francis],20180828, must config backup partition
static quec_backup_info_type Flag_msg_init = {0,0,0,0,0,{0},0,0,0,{0},0,0,0,{0},0,0,{0},0,0,
    {
//Wythe.WANG-20190521: Add EC20CE_FDG project followling EC20CE_FAG configuration,
//Wythe.WANG-20190527: Add EC20CE_FDG_CTVT project followling EC20CE_FDG configuration,
#if defined(CONFIG_PROJECT_EC20CE_FAG)  || defined(CONFIG_PROJECT_EC20CE_FDG) || defined(CONFIG_PROJECT_EC21KL_KR) || defined(CONFIG_PROJECT_EC25AU) || defined(CONFIG_PROJECT_EC20CE_FARG) || defined(CONFIG_PROJECT_EC20CE_FRG) || defined(CONFIG_PROJECT_EC21AU) \
        || defined(CONFIG_PROJECT_EC21KL) || defined(CONFIG_PROJECT_EC20CE_FDG_CTVT) || defined(CONFIG_PROJECT_EC25E) || defined(CONFIG_PROJECT_EG92EU) || defined(CONFIG_PROJECT_EG95EB) || defined(CONFIG_PROJECT_EC25E_EMBMS) || defined(CONFIG_PROJECT_EC25EU) || defined(CONFIG_PROJECT_EC25EU_GB)  \
        || defined(CONFIG_PROJECT_EC21EU) || defined(CONFIG_PROJECT_EG91EX_GB) || defined(CONFIG_PROJECT_EG92EU_DP) || defined(CONFIG_PROJECT_EC21E) || defined(CONFIG_PROJECT_EG91EB)
        {"modem", "qdsp6sw_b", 0,0,0,0,0,0},
        {"system", "sys_back", 0,0,0,0,0,0},
        {"recoveryfs", "recoveryfs_b", 0,0,0,0,0,0},
        {"usr_data", "", 0,0,0,0,0,0},
        {"data", "", 0,0,0,0,0,0}
#elif defined(CONFIG_PROJECT_EC20CE_FASG) || defined(CONFIG_PROJECT_EC20CE_FASG_HW) \
  || defined(CONFIG_PROJECT_EC20CE_FASG_OCL) || defined (CONFIG_PROJECT_EC25EM_GC) || defined(CONFIG_PROJECT_EC20CE_FASG_CTVT) \
  || defined (CONFIG_PROJECT_EC21AUX_GA) || defined (CONFIG_PROJECT_EC25EM_GB) || defined (CONFIG_PROJECT_EC21AUX_GAA) || defined (CONFIG_PROJECT_EC21AUX_GAD) || defined (CONFIG_PROJECT_EC25AUX_GA) || defined (CONFIG_PROJECT_EC25AUX_GAA)  \
  || defined (CONFIG_PROJECT_EC25MX) || defined (CONFIG_PROJECT_EG91EX) || defined (CONFIG_PROJECT_EG91_JP) || defined (CONFIG_PROJECT_EG95_JP) || defined (CONFIG_PROJECT_EG91EX_GAA) || defined (CONFIG_PROJECT_EG91EX_REL) \
  || defined (CONFIG_PROJECT_EG95EX)|| defined (CONFIG_PROJECT_EG95EX_GAA)|| defined (CONFIG_PROJECT_EC25EUX) \
  || defined (CONFIG_PROJECT_EC21EUX) || defined (CONFIG_PROJECT_EC25EX) || defined (CONFIG_PROJECT_EC20CE_HKLG) \
  || defined (CONFIG_PROJECT_EC20CE_HDLG_CTVT) || defined (CONFIG_PROJECT_EC20CE_HDLG) || defined (CONFIG_PROJECT_EC20CE_HCLG) || defined (CONFIG_PROJECT_EC20CE_HHLG) || defined (CONFIG_PROJECT_EM05CN_FD) || defined (CONFIG_PROJECT_EM05CN_FDMBIM) \
  || defined (CONFIG_PROJECT_EC20CE_FALG) || defined (CONFIG_PROJECT_EC20CE_FILG) || defined (CONFIG_PROJECT_EC20CE_FHLG) || defined (CONFIG_PROJECT_EC20CE_FDLG)\
  || defined (CONFIG_PROJECT_EG95AUX) || defined (CONFIG_PROJECT_EG91AUX) || defined (CONFIG_PROJECT_EG95AUX_GAD) || defined (CONFIG_PROJECT_EG91AUX_GAD) || defined (CONFIG_PROJECT_EG95AUX_DP)\
  || defined (CONFIG_PROJECT_EC25EX_GC)|| defined(CONFIG_PROJECT_EC20CE_FRSG) || defined(CONFIG_PROJECT_EC25EB_GB) || defined(CONFIG_PROJECT_EC25EUC) || defined(CONFIG_PROJECT_EC25EUC_TP)
        {"usr_data", "", 0,0,0,0,0,0}

#else
#error "please add your project name from your partition.xml"
#endif
    }
};

struct qstart_device_t{
	struct miscdevice misc;
};

typedef struct 
{
  uint32_t magic1;  
  uint32_t magic2;
  uint32_t page_count;
  uint32_t data_crc;
  
  uint32_t reserve1;  
  uint32_t reserve2;
  uint32_t reserve3;
  uint32_t reserve4;
} quec_cefs_file_header_type;


static struct mtd_info *mtd = NULL;
static loff_t qfirst_goodblock_addr = 0;

struct qstart_device_t *qstart_devp;
struct timer_list qstart_poll_timer;
struct work_struct qstart_timer_work;

static DEFINE_MUTEX(qstart_timer_lock);

static int start_mode_set(const char *val, struct kernel_param *kp);
static int start_mode = 0;
module_param_call(start_mode, start_mode_set, param_get_int, &start_mode, 0644);

// add by hans @20201204 "add write_forbidden attribute file to forbidden MTD write" start
extern unsigned int ql_get_nand_layer_write_forbidden(void);
// add by hans @20201204 "add write_forbidden attribute file to forbidden MTD write" end

bool boot_mode = false;
bool debug_mode = false;
//add by [francis],20180814,add debug kernel for opening in aboot

//add by len, get the recovery value from cmdline, 2018-1-18
static int __init quectel_set_bootmode(char *str)
{
    if (str) {
        boot_mode = true;
    }
    else {
        boot_mode = false;
    }
    return 0;
}

static int __init quectel_set_debugmode(char *str)
{
    if (str) {
        debug_mode = true;
    }
    else {
        debug_mode = false;
    }
    return 0;
}

early_param("recovery", quectel_set_bootmode);
early_param("debug", quectel_set_debugmode);

/**
 * Author : Darren
 * Date : 2017/6/30
 * get_bootmode - will return current start mode
 * 0 -- normode, system mount linuxfs as rootfs
 * 1 -- recoverymode, system mount recoveryfs as rootfs
 */
//modify by len 2018-1-18 for boot mode
bool get_bootmode(unsigned int *mode)
{
    return boot_mode;
}
//modify end
EXPORT_SYMBOL(get_bootmode);

bool get_debugmode(unsigned int *mode)
{
    return debug_mode;
}
EXPORT_SYMBOL(get_debugmode);

//add by [francis],20190715, set recovery-flag in misc for ota
struct bootloader_message {
    char command[32];
    char status[32];
    char boot_switch[32];
    char first_in[32];
    char recovery[992];
    char backup_flag[32];
};

struct bootloader_message boot;
static const int MISC_PAGES = 3;                        // number of pages to save
static const int MISC_COMMAND_PAGE = 1; 

void ql_write_flag_in_misc(void)
{
	struct mtd_info *mtd = NULL;
	size_t write = 0;
	struct erase_info ei;
	int err = 0, n =0;

	unsigned char *bufpage = NULL;

	memset(&boot, 0, sizeof(boot));
	strlcpy(boot.command,"boot-recovery",sizeof(boot.command));
	strlcpy(boot.recovery,"recovery\n",sizeof(boot.recovery));
	strlcpy(boot.first_in,"first\n",sizeof(boot.first_in));
	strlcpy(boot.backup_flag,"fail",sizeof(boot.backup_flag));


	mtd = get_mtd_device_nm("misc");
	if(IS_ERR(mtd))
	{
		printk("@quectel  get misc mtd fail.!\r\n");
		return 0;
	}
	else
	{
		n = mtd->writesize * (MISC_COMMAND_PAGE + 1);
		bufpage =  kmalloc(n, GFP_KERNEL);
		memset(bufpage,0x00,n);
		memcpy((void *)(bufpage+mtd->writesize),(void *)&boot,sizeof(boot));
		memset(&ei, 0, sizeof(struct erase_info));
		ei.mtd = mtd;
		ei.addr = 0;
		ei.len = mtd->erasesize;
		err = mtd_erase(mtd, &ei); //炔一要写

		err = mtd_write(mtd, 0, n , &write, bufpage);
		if(err || write != n)
		{	
		   printk("@quectel  get misc mtd fail.!\r\n");
		   return 0;
		}

		 printk("@quectel  set misc mtd ok.!boot.command  =%s  \r\n",boot.command);
	}
}


static int start_mode_set(const char *val, struct kernel_param *kp)
{
	int ret;

	ret = param_set_int(val,kp);

	if(ret)
		return ret;
	
	return 0;
}

static void qstart_poll_timer_cb(void)
{
    static unsigned int count = 0;

	mutex_lock(&qstart_timer_lock);
	count++;
    if(count >=  QUEC_START_TIMER_TOUT && 0 == start_mode)
    {
		printk("\n\n\nStart mode = %d, count=%d\n\n\n", start_mode,count);
		schedule_work(&qstart_timer_work);	
   		//panic("Quec Start Timer error!");     
    }
	mutex_unlock(&qstart_timer_lock);

	if(1 == start_mode)
		del_timer(&qstart_poll_timer);
	else
 		mod_timer(&qstart_poll_timer,jiffies + HZ);
}

static struct qstart_device_t qstart_device = {
	.misc = {
		.minor = MISC_DYNAMIC_MINOR,
		.name = "qstart",
	}
};
/******************************************************************************************
who-2018/06/25:Description....
Refer to [Issue-Depot].[IS0000197][Submitter:ramos.zhang,Date:2018-06-25]
<Ä£¿éÏÖ³¡efsËð»µÎÞ·š»¹Ô­modem²»Í£ÖØÆô»òÕßcfun=7ÎÞ·šÉÏÍø>
******************************************************************************************/
unsigned int Quectel_Is_EFS_Backup_Valid(void)
{
	size_t readlen = 0;
	quec_cefs_file_header_type BackupCefs_Info;
	uint32_t crc = 0;
	uint32_t i =0;
	uint64_t mtd_size;
	unsigned char *onepage = NULL;

	if(NULL == mtd) // Ramos.zhang 20180624, add efs restore detect,  
		mtd = get_mtd_device_nm(SYS_REV_PARTITION_NAME);
	if(IS_ERR(mtd))
	{
		printk("@Ramos get sys_rev mtd fail.!\r\n");
		goto efsBackupInvalid;
	}
	else
	{
		mtd_size = mtd->size;
		printk("@Ramos mtd->writesize =%d, mtd->erasesize:%d  blockcount\n",  mtd->writesize,mtd->erasesize);
		for(i=0; qfirst_goodblock_addr < mtd_size; i++)
		{
			qfirst_goodblock_addr = i * mtd->erasesize; 
			if(!mtd_block_isbad(mtd,qfirst_goodblock_addr)) 
				break;
		}

		onepage = kmalloc(mtd->writesize, GFP_KERNEL);
		if(NULL == onepage)
		{
			printk("@Ramos memory is not enough to onepage, line=%d\n", __LINE__);
			goto  efsBackupInvalid;
		}
		
		memset(onepage, 0x00, mtd->writesize);
		mtd_read(mtd, qfirst_goodblock_addr, mtd->writesize, &readlen ,onepage);
		if(readlen != mtd->writesize )
		{
			printk("@Ramos read Flag Failed!!,line=%d\r\n\r\n",__LINE__);
			goto  efsBackupInvalid;
		}
		memset((void *)&BackupCefs_Info, 0x00, sizeof(quec_cefs_file_header_type));
		memcpy((void *)&BackupCefs_Info, onepage, sizeof(quec_cefs_file_header_type));

#ifdef QUECTEL_RUSSIAN_FORBIDDEN_FW_ROLLBACK_FEATURE
		if(
        ((CEFS_FILE_MAGIC1 != BackupCefs_Info.magic1) || (CEFS_FILE_MAGIC2 != BackupCefs_Info.magic2))
		 && ((NEW_CEFS_FILE_MAGIC1 != BackupCefs_Info.magic1) || (NEW_CEFS_FILE_MAGIC2 != BackupCefs_Info.magic2))
         )
#else
		if((CEFS_FILE_MAGIC1 != BackupCefs_Info.magic1) || (CEFS_FILE_MAGIC2 != BackupCefs_Info.magic2))
#endif
		{
			printk("@Ramos efs2 restore file magic1 error !!!\r\n\r\n");
			goto  efsBackupInvalid;
		}
	}

	return 1;
efsBackupInvalid:
	if(onepage != NULL)
	{
		kfree(onepage);
		onepage = NULL;
	}
	return 0;
}


unsigned int Quectel_Is_BackupPartition_Exist(const char * sourc_partition)
{
	struct mtd_info *mtd = NULL;

    mtd = get_mtd_device_nm(sourc_partition);
	if(IS_ERR(mtd))
	{
		printk("@Ramos can't get backup partition !!!\r\n");
		return 0;
	}
    else
    {
        if(!strcmp(sourc_partition, "efs2"))
	    {
	    	if(Quectel_Is_EFS_Backup_Valid())
	    	{
	    		return 1;
	    	}
	    	else
	    	{
	    		return 0;
	    	}
	    }
    }
}

unsigned int Quectel_Set_Partition_RestoreFlag_no_reboot(const char * partition_name,int mtd_nub, int where)
{
	unsigned char *onepage = NULL;
	quec_backup_info_type Flag_msg;
	struct erase_info ei;
	size_t write = 0;
	size_t readlen = 0;
	int err,i;
	int ret = 1;
	uint64_t tmp;
	uint32_t blocksize=0;
	int isExistFlag = 0;

/****************************************************************************
Refer to [Issue-Depot].[IS0001189][Submitter:francis.huan,Data:2020-05-18]
****************************************************************************/
#if 0
	if (!Quectel_Is_BackupPartition_Exist(partition_name))
	{
		printk("@Ramos the [%s] partition no backup partitino  or  efs backup Invalid !!!!!\r\n", partition_name);
		return 0;
	}
#endif

	// add by hans @20201204 "add write_forbidden attribute file to forbidden MTD write" start
	if(ql_get_nand_layer_write_forbidden() == 1)
	{
		return 0;
	}
	// add by hans @20201204 "add write_forbidden attribute file to forbidden MTD write" end

	//  sys_rev 写flagbootloader 时flag卸欠原应姆
	mtd = get_mtd_device_nm(SYS_REV_PARTITION_NAME);
	if(IS_ERR(mtd))
	{
		printk("@Ramos get sys_rev mtd fail.!\r\n");
		return 0;
	}
	else
	{
		tmp = mtd->size; // totle size .
		blocksize =  mtd->erasesize;
		
		printk("@Ramos :mtdsize:%llx, mtd->writesize =%d, mtd->erasesize:%d  blockcount\n", tmp, mtd->writesize,mtd->erasesize);
		for(i=0; i<QUEC_BACKUP_INFO_BLOCK_NUMS; i++)
		{
			qfirst_goodblock_addr =tmp -(QUEC_BACKUP_INFO_BLOCK_NUMS- i) * mtd->erasesize; // 要要偏疲始哟flag牡址block
			if(!mtd_block_isbad(mtd,qfirst_goodblock_addr))	
				break;
		}

		onepage = kmalloc(mtd->writesize, GFP_KERNEL);
		if(NULL == onepage)
		{
			printk("@Ramos memory is not enough to onepage\n");
			return ret;
		}
		
		printk("@Ramos qfirst_goodblock_addr=%d \n", qfirst_goodblock_addr);
		memset(onepage, 0x00, mtd->writesize);
		mtd_read(mtd, qfirst_goodblock_addr, mtd->writesize, &readlen ,onepage);
		if(readlen != mtd->writesize )
		{
			printk("@Ramos read Flag Failed!!\r\n\r\n");
		}

		// 录莼原
                memcpy((void *)&Flag_msg,(void *)onepage,sizeof(Flag_msg));
		printk("@Ramos set partition(%s) restore, offset=%lx,print magic1=%lx,magic2=%lx\r\n", partition_name, qfirst_goodblock_addr,Flag_msg.magic1,Flag_msg.magic2);

		int i;			
		for ( i = 0 ;i < 6;i++)
		{
			if(Flag_msg_init.ql_mtd_info[i].ql_mtd_name[0] != 0)
			{
				if(!IS_ERR(get_mtd_device_nm(Flag_msg_init.ql_mtd_info[i].ql_mtd_name))){
					Flag_msg_init.ql_mtd_info[i].ql_mtd_nub=get_mtd_device_nm(Flag_msg_init.ql_mtd_info[i].ql_mtd_name)->index;
					Flag_msg_init.ql_mtd_info[i].total_size=get_mtd_device_nm(Flag_msg_init.ql_mtd_info[i].ql_mtd_name)->size;
				}
				// get mtd nub for src partition
				if(true)
				{
					strcpy(Flag_msg.ql_mtd_info[i].ql_mtd_name,Flag_msg_init.ql_mtd_info[i].ql_mtd_name);
					strcpy(Flag_msg.ql_mtd_info[i].ql_mtd_restore_name,Flag_msg_init.ql_mtd_info[i].ql_mtd_restore_name);
					Flag_msg.ql_mtd_info[i].ql_mtd_nub =  Flag_msg_init.ql_mtd_info[i].ql_mtd_nub;
					Flag_msg.ql_mtd_info[i].total_size =  Flag_msg_init.ql_mtd_info[i].total_size;					
				}
			}else {	
				strcpy(Flag_msg.ql_mtd_info[i].ql_mtd_name,"");
			}
		}
		Flag_msg.magic1 =  QUEC_BACKUP_MAGIC1;
		Flag_msg.magic2 =  QUEC_BACKUP_MAGIC2;
                Flag_msg.data_magic1 =  QUEC_DATA_MAGIC1;
                Flag_msg.data_magic2 =  QUEC_DATA_MAGIC2;	
		if(!strcmp(partition_name,"efs2"))
		{
			Flag_msg.cefs_restore_flag = 0x10;
			isExistFlag = 1;
		}

		printk("@Ramos check partition info  over !!!!!\n" );
			for ( i = 0 ;i < 6;i++)
			{
				if(Flag_msg.ql_mtd_info[i].ql_mtd_name[0] != 0)
				{
					if( partition_name[0] == 0 )
					{
						if(Flag_msg.ql_mtd_info[i].ql_mtd_nub == mtd_nub)
						{
							if(!Quectel_Is_BackupPartition_Exist(Flag_msg.ql_mtd_info[i].ql_mtd_name))
						{
							printk("the [%s] parition no exist !!!\n",Flag_msg.ql_mtd_info[i].ql_mtd_name);
							return 0;
						}	
							Flag_msg.ql_mtd_info[i].restore_flag = 1;
							Flag_msg.ql_mtd_info[i].crash[where] +=1;
							isExistFlag = 1;
						}
					}else
					{
						if(!strcmp(Flag_msg.ql_mtd_info[i].ql_mtd_name,partition_name))
						{
							if(!Quectel_Is_BackupPartition_Exist(Flag_msg.ql_mtd_info[i].ql_mtd_name))
						{
							printk("the [%s] parition no exist !!!\n",Flag_msg.ql_mtd_info[i].ql_mtd_name);
							return 0;
						}	
							Flag_msg.ql_mtd_info[i].restore_flag = 1;
							Flag_msg.ql_mtd_info[i].crash[where] +=1;
							isExistFlag = 1;
						}
					}
				}	

		}
		
		if (isExistFlag == 0)
		{
			printk("Waring:Restore partition is not exist\n");
			return 0;
		}
		memset(&ei, 0, sizeof(struct erase_info));
		ei.mtd = mtd;
		ei.addr = qfirst_goodblock_addr;
		ei.len = mtd->erasesize;
		err = mtd_erase(mtd, &ei); //炔一要写
		
                memcpy((void *)onepage,(void *)&Flag_msg, sizeof(Flag_msg));
		err = mtd_write(mtd, qfirst_goodblock_addr, mtd->writesize, &write, onepage);
		if(err || write != mtd->writesize)
		{
			printk("@Ramos set partition(%s) Flag  failed at %#llx\n", partition_name,(long long)qfirst_goodblock_addr);
			goto exit;
		}
/*
		memset(onepage, 0x00, mtd->writesize);
		mtd_read(mtd, qfirst_goodblock_addr, mtd->writesize, &readlen ,onepage);
		printk("@Ramos debug Restore flag is:%s\n",  ((struct restorflag_message *)onepage)->command);
*/
		
	}	
	ret = 1;
exit:
	if(onepage != NULL)
	{
		kfree(onepage);
		onepage = NULL;
	}
	return ret;
}


// modify by [francis.huan],20180417,priortity partition_name > mtd_nub,then setting RestoreFlag
unsigned int Quectel_Set_Partition_RestoreFlag(const char * partition_name, int mtd_nub, int where)
{
    unsigned char *onepage = NULL;
    quec_backup_info_type Flag_msg;
    struct erase_info ei;
    size_t write = 0;
    size_t readlen = 0;
    int err,i;
    int ret = 0;
    uint64_t tmp;
    //uint32_t blocksize=0;
/******************************************************************************************
francis-2018/11/11:Description....
Refer to [Issue-Depot].[IS0000336][Submitter:francis.huan,Date:2018-11-10]
<新备份还原机制中在配置表中没有任何配置的分区若触发还原点由默认重启改为默认不做任何操作>
******************************************************************************************/
    int isExistFlag = 0;
    struct mtd_info *mtd = NULL;

    if(where>11)
    {
	printk("ERROR !!!!!!! @Quectel0125 the value of where is bigger than 11, will change the value of where to 0\r\n");
	where = 0;
    }

	// add by hans @20201204 "add write_forbidden attribute file to forbidden MTD write" start
	if(ql_get_nand_layer_write_forbidden() == 1)
	{
		return 0;
	}
	// add by hans @20201204 "add write_forbidden attribute file to forbidden MTD write" end

	mtd = get_mtd_device_nm(SYS_REV_PARTITION_NAME);
	if(IS_ERR(mtd))
	{
		printk("@Ramos get sys_rev mtd fail.!\r\n");
		return 0;
	}
	else
	{
		tmp = mtd->size; // totle size .
		//blocksize =  mtd->erasesize;
		
		printk("@Ramos :mtdsize:%llx, mtd->writesize =%d, mtd->erasesize:%d  blockcount\n", tmp, mtd->writesize,mtd->erasesize);
		for(i=0; i<QUEC_BACKUP_INFO_BLOCK_NUMS; i++)
		{
			qfirst_goodblock_addr =tmp -(QUEC_BACKUP_INFO_BLOCK_NUMS- i) * mtd->erasesize; // ҪҪƫƣʼӴflagĵַblock
			if(!mtd_block_isbad(mtd,qfirst_goodblock_addr)) 
				break;
		}

		onepage = kmalloc(mtd->writesize, GFP_KERNEL);
		if(NULL == onepage)
		{
			printk("@Ramos memory is not enough to onepage\n");
			return ret;
		}
		
		printk("@Ramos qfirst_goodblock_addr=%d \n", qfirst_goodblock_addr);
		memset(onepage, 0x00, mtd->writesize);
		mtd_read(mtd, qfirst_goodblock_addr, mtd->writesize, &readlen ,onepage);
		if(readlen != mtd->writesize )
		{
			printk("@Ramos read Flag Failed!!\r\n\r\n");
		}

        // ¼ݻԭ
        memcpy((void *)&Flag_msg,(void *)onepage,sizeof(Flag_msg));
        printk("@Ramos set partition(%s) restore, offset=%lx,print magic1=%lx,magic2=%lx\r\n", partition_name, qfirst_goodblock_addr,Flag_msg.magic1,Flag_msg.magic2);

        for(i=0; i < 6;i++)
		{
            if (Flag_msg_init.ql_mtd_info[i].ql_mtd_name[0] != 0)
            {
                if (!IS_ERR(get_mtd_device_nm(Flag_msg_init.ql_mtd_info[i].ql_mtd_name))){
                    Flag_msg_init.ql_mtd_info[i].ql_mtd_nub = get_mtd_device_nm(Flag_msg_init.ql_mtd_info[i].ql_mtd_name)->index;
                    Flag_msg_init.ql_mtd_info[i].total_size = get_mtd_device_nm(Flag_msg_init.ql_mtd_info[i].ql_mtd_name)->size;
                }
                //get mtd nub for src partition
                strcpy(Flag_msg.ql_mtd_info[i].ql_mtd_name, Flag_msg_init.ql_mtd_info[i].ql_mtd_name);
                strcpy(Flag_msg.ql_mtd_info[i].ql_mtd_restore_name, Flag_msg_init.ql_mtd_info[i].ql_mtd_restore_name);
                Flag_msg.ql_mtd_info[i].ql_mtd_nub = Flag_msg_init.ql_mtd_info[i].ql_mtd_nub;
                Flag_msg.ql_mtd_info[i].total_size = Flag_msg_init.ql_mtd_info[i].total_size;
		    }else {
                strcpy(Flag_msg.ql_mtd_info[i].ql_mtd_name, "");
            }
        }
        Flag_msg.magic1 = QUEC_BACKUP_MAGIC1;
        Flag_msg.magic2 = QUEC_BACKUP_MAGIC2;

		Flag_msg.data_magic1 =  QUEC_DATA_MAGIC1;
		Flag_msg.data_magic2 =  QUEC_DATA_MAGIC2;
	    if(!strcmp(partition_name, "efs2"))
	    {
	        Flag_msg.cefs_restore_flag = 0x10;
		isExistFlag = 1; //Ramos.zhang-20190308 resolve efs2 paritiion can't restore when modme crash 4 times
		}
        printk("@Ramos check partition info  over !!!!!\n");
        for (i = 0; i < 6; i++)
        {
            if (Flag_msg.ql_mtd_info[i].ql_mtd_name[0] != 0)
            {
                if( partition_name[0] == 0 )
                {
                    if(Flag_msg.ql_mtd_info[i].ql_mtd_nub == mtd_nub)
                    {
                        if(!Quectel_Is_BackupPartition_Exist(Flag_msg.ql_mtd_info[i].ql_mtd_name))
                        {
                            printk("the [%s] partition no exist !!!\n", Flag_msg.ql_mtd_info[i].ql_mtd_name);
                            return 0;
                        }
                        Flag_msg.ql_mtd_info[i].restore_flag = 1;
                        Flag_msg.ql_mtd_info[i].crash[where] += 1;
			isExistFlag = 1;
                    }
                }else
                {
                    if(!strcmp(Flag_msg.ql_mtd_info[i].ql_mtd_name, partition_name))
                    {
                        if(!Quectel_Is_BackupPartition_Exist(Flag_msg.ql_mtd_info[i].ql_mtd_name))
                        {
                            printk("the [%s] partition no exist !!!\n", Flag_msg.ql_mtd_info[i].ql_mtd_name);
                            return 0;
                        }
                        Flag_msg.ql_mtd_info[i].restore_flag = 1;
                        Flag_msg.ql_mtd_info[i].crash[where] += 1;
			isExistFlag = 1;
                    }
                }
            }

        }

	
		if (isExistFlag == 0)
		{
			printk("Waring:Restore partition is not exist\n");
			return 0;
		}

		memset(&ei, 0, sizeof(struct erase_info));
		ei.mtd = mtd;
		ei.addr = qfirst_goodblock_addr;
		ei.len = mtd->erasesize;
		err = mtd_erase(mtd, &ei); //先擦除一下这个要写的区域\E3\80?
		
				memcpy((void *)onepage,(void *)&Flag_msg, sizeof(Flag_msg));
		err = mtd_write(mtd, qfirst_goodblock_addr, mtd->writesize, &write, onepage);
		if(err || write != mtd->writesize)
		{
			printk("@Ramos set partition(%s) Flag  failed at %#llx\n", partition_name,(long long)qfirst_goodblock_addr);
			goto exit;
		}
/*
        memset(onepage, 0x00, mtd->writesize);
        mtd_read(mtd, qfirst_goodblock_addr, mtd->writesize, &readlen ,onepage);
        printk("@Ramos debug Restore flag is:%s\n",  ((struct restorflag_message *)onepage)->command);
*/
        
    }    
    ret = 1;
    machine_restart(NULL);
exit:
    if(onepage != NULL)
    {
        kfree(onepage);
        onepage = NULL;
    }
    return ret;
}

void Quectel_Partition_Restore(const char* partition_name, int mtd_nub, int where)
{
    Quectel_Set_Partition_RestoreFlag(partition_name, mtd_nub, where);
}

void Quectel_Erase_Partition(const char * partition_name)
{
	struct mtd_info *mtd = NULL;
	struct erase_info ei;
	int err,i;
	
	// add by hans @20201204 "add write_forbidden attribute file to forbidden MTD write" start
	if(ql_get_nand_layer_write_forbidden() == 1)
	{
		return;
	}
	// add by hans @20201204 "add write_forbidden attribute file to forbidden MTD write" end

	printk("@Ramos there are fatal errror on the %s partition , we must erase it !!!\r\n", partition_name);
	mtd = get_mtd_device_nm(partition_name);
	if(IS_ERR(mtd))
	{
		printk("ERROR!!!!!  @Ramos get  %s mtd fail.!\r\n", partition_name);
		return 0;
	}
	else
	{
		memset(&ei, 0, sizeof(struct erase_info));
		ei.mtd = mtd;
		ei.len = mtd->erasesize;
		for(i=0 ;  ; i++)
		{
			ei.addr = i*(mtd->erasesize);
			if(ei.addr  > mtd->size)
			{
				break;
			}
			err = mtd_erase(mtd, &ei); //
		}
	}
		
	machine_restart(NULL);

return 0;
}

EXPORT_SYMBOL(Quectel_Set_Partition_RestoreFlag);
EXPORT_SYMBOL(Quectel_Partition_Restore);
EXPORT_SYMBOL(Quectel_Erase_Partition);

static void qstart_timer_handle(struct work_struct *work)
{
	Quectel_Set_Partition_RestoreFlag("system",-1,10);
}

static void __exit qstart_exit(void)
{
	misc_deregister(&qstart_device.misc);
}
static ssize_t qstart_cefs_flag_store (struct device *dev,struct device_attribute *attr,
					 char *buf, size_t size)
{
	Quectel_Set_Partition_RestoreFlag_no_reboot("efs2",-1,9);
	return size;
}
static int quectel_alloc_smem_modem(const char *mcfg)
{
	void * data_ptr = NULL;
	data_ptr = smem_alloc(SMEM_ID_VENDOR2,
				512, 0,
				SMEM_ANY_HOST_FLAG);
        if(data_ptr!=NULL)
    	  memcpy(data_ptr,mcfg,strlen(mcfg)+1);
	else
	  return -ENOMEM;
	return 0;
}
static ssize_t qstart_recovery_flag_store (struct device *dev,struct device_attribute *attr,
                                         char *buf, size_t size)
{
	quectel_alloc_smem_modem("recovery");
	ql_write_flag_in_misc();
        return size;
}
static DEVICE_ATTR(qstart_cefs_flag,0644,NULL,qstart_cefs_flag_store);
static DEVICE_ATTR(qstart_recovery_flag,0644,NULL,qstart_recovery_flag_store);

static int qstart_probe (struct platform_device *pdev)
{
	if( device_create_file(&pdev->dev,&dev_attr_qstart_cefs_flag) < 0)
	{
		printk("qstart create attr failed \n");
	}else{
		printk("qstart create attr success\n");
	}

        if( device_create_file(&pdev->dev,&dev_attr_qstart_recovery_flag) < 0)
        {
                printk("qstart create attr failed \n");
        }else{
                printk("qstart create attr success\n");
        }

	return 0;
}

static const struct of_device_id qstart_match[] = 
{
	{.compatible = "quec,quectel-qstart-config",},
	{},
};
static struct platform_driver qstart_driver = {
        .driver = {
                .name = "qstart",
		.of_match_table = qstart_match, 
        },
        .probe = qstart_probe,
};

static int __init qstart_init(void)
{
	int ret = 0;

	printk("@Ramos Qstart_init entry !!!\r\n");

	return platform_driver_register(&qstart_driver);
#if 0   //QUECTEL_LINUX_APP_DETECT
	qstart_devp = &qstart_device;
	init_timer(&qstart_poll_timer);
                qstart_poll_timer.function = (void *)qstart_poll_timer_cb; 
                qstart_poll_timer.expires = jiffies + HZ;
                add_timer(&qstart_poll_timer);
	
	INIT_WORK(&qstart_timer_work, qstart_timer_handle);		
	
	ret = misc_register(&qstart_device.misc);
#endif
	return ret;
}

module_init(qstart_init);
module_exit(qstart_exit);

MODULE_DESCRIPTION("QUECTEL Start Driver");
MODULE_LICENSE("GPL v2");

#endif

