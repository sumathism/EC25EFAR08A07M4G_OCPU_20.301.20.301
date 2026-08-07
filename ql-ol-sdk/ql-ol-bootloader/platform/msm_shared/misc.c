#include <debug.h>
#include <string.h>
#include <misc.h>
#include <sys/types.h>
#include <platform.h>
#include <dev/flash.h>
#include <qpic_nand.h>

//add by dawn for set misc flag 2018-9-04
int get_sysconfig_from_misc(Enum_SysCfgId syscfgid, ST_SystemConfig *systemcfg)
{
	//dprintf(CRITICAL, "[Dawn] get syscfgid=0x%x \n", syscfgid);
	struct ptentry *ptn;
	struct ptable *ptable;
	unsigned pagesize = flash_page_size();
	unsigned blocksize = flash_block_size();
	int i, lenth;
	#if 1
	int datalen=(((SYS_CONFIG_END-SYS_CONFIG_START)*sizeof(ST_SystemConfig))/pagesize+1)*pagesize;
#else
	int datalen = blocksize;
#endif
	char *block_buffer = NULL;
	ST_SystemConfig *tmp_systemcfg = NULL;


	ptable = flash_get_ptable();

	if (ptable == NULL) {
		dprintf(CRITICAL, "[Dawn] ERROR: Partition table not found\n");
		return -1;
	}

	ptn = ptable_find(ptable, "misc");

	if (ptn == NULL) {
		dprintf(CRITICAL, "[Dawn] ERROR: No misc partition found\n");
		return -1;
	}

	dprintf(CRITICAL, "[Dawn] alloc len: %d block len: %d pagesize=:%d\n",datalen,blocksize,pagesize);
		block_buffer = (char *)malloc(datalen);
	if(block_buffer == NULL) {
		dprintf(CRITICAL, "[Dawn] ERROR: Cannot malloc block_buffer\n");
		return -1;
	}
	if(Quectel_flash_nand_read(ptn, 0, 0, block_buffer, datalen))
	{
		dprintf(CRITICAL, "[Dawn] ERROR: Cannot read Block\n");
		free(block_buffer);
		return -1;
	}
	tmp_systemcfg = (ST_SystemConfig *)block_buffer;
	tmp_systemcfg += syscfgid-SYS_CONFIG_START;
 
	//dprintf(CRITICAL, "[Dawn-get] tmp_systemcfg->itemId = 0x%x \n", tmp_systemcfg->itemId);
	if(tmp_systemcfg->itemId == syscfgid) {
		memset((void *)systemcfg, 0, sizeof(ST_SystemConfig));
		memcpy((void *)systemcfg, (void *)tmp_systemcfg, sizeof(ST_SystemConfig));
		dprintf(CRITICAL, "[Dawn] 0x%x exist\n", tmp_systemcfg->itemId);
		free(block_buffer);
		return 0;
	}
 
	dprintf(CRITICAL, "[Dawn] 0x%x not exist,Pllease set item before!!\n", syscfgid);
	free(block_buffer);
	return -1;

}

int set_sysconfig_to_misc(Enum_SysCfgId syscfgid, ST_SystemConfig *systemcfg)
{

	dprintf(CRITICAL, "[Dawn]: set syscfgid=0x%x\n",syscfgid);

	struct ptentry *ptn;
	struct ptable *ptable;
	unsigned pagesize = flash_page_size();
	uint32_t blocksize = flash_block_size();
	int i;
	#if 1
		int datalen = (((SYS_CONFIG_END-SYS_CONFIG_START)*sizeof(ST_SystemConfig))/pagesize+1)*pagesize;
#else
		int datalen = blocksize;
#endif

	char *block_buffer = NULL;
	ST_SystemConfig *tmp_systemcfg = NULL;

	ptable = flash_get_ptable();

	if (ptable == NULL) {
		dprintf(CRITICAL, "ERROR: Partition table not found\n");
		return -1;
	}
	ptn = ptable_find(ptable, "misc");

	if (ptn == NULL) {
		dprintf(CRITICAL, "ERROR: No misc partition found\n");
		return -1;
	}

	dprintf(CRITICAL, "[Dawn] alloc len: %d\n",datalen);
	block_buffer = (char *)malloc(datalen);
	if(block_buffer == NULL) {
		dprintf(CRITICAL, "[Dawn] ERROR: Cannot malloc block_buffer\n");
		return -1;
	}
	if(Quectel_flash_nand_read(ptn, 0, 0, block_buffer, datalen))
	{
		dprintf(CRITICAL, "[Dawn] ERROR: Cannot read Block\n");
		free(block_buffer);
		return -1;
	}
	tmp_systemcfg = (ST_SystemConfig *)block_buffer;
	tmp_systemcfg += syscfgid-SYS_CONFIG_START;
 
	dprintf(CRITICAL, "[Dawn-set] tmp_systemcfg->itemId=0x%x\n",tmp_systemcfg->itemId);
	memset((void *)tmp_systemcfg, 0, sizeof(ST_SystemConfig));
	systemcfg->itemId = syscfgid;
	memcpy((void *)tmp_systemcfg, (void *)systemcfg, sizeof(ST_SystemConfig));
 
 	dprintf(CRITICAL, "[Dawn] write sysconfig message to flash NOW......\n");
	if (Quectel_flash_write(ptn,0, 0, block_buffer, datalen)) 
	{
		dprintf(CRITICAL, "ERROR: SYSCONFIG flash write fail!\n");
		free(block_buffer);
		return -1;
	}
	dprintf(CRITICAL, "[Dawn] write sysconfig message to flash END !!!\n");
	free(block_buffer);
	return 0;

}
