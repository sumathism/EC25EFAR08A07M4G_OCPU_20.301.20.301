/*==========================================================================
 |                    QUECTEL - Build a smart world.
 |
 |               Copyright(c) 2017 QUECTEL Incorporated.
 |
 |--------------------------------------------------------------------------
 | File Description
 | ----------------
 | quectel raw data
 |
 |--------------------------------------------------------------------------
 |
 | Designed by: will.shao
 |--------------------------------------------------------------------------
 | Revision History
 | ----------------
 | 2018/05/04               will.shao               Initialization
 | 2018/11/18               Mike                    Modify data type
 | 2019/02/02               Mike                    Adds __attribute__((packed)) in raw_data_list_type
 | ------------------------------------------------------------------------
 \=========================================================================*/
#include <linux/seq_file.h>
#include <linux/proc_fs.h>
#include <linux/types.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/capability.h>
#include <linux/export.h>
#include <linux/mutex.h>
#include <asm/uaccess.h>
#include <linux/quectel/quec_flash.h>
#include <linux/quectel/quec_raw_flash_data.h>
#include "quec_crc.h"

#define RAW_DATA_CACHE_SIZE 		(8192)
#define QUECTEL_USB_FULL_SPEED_CLOSE    0
#define QUECTEL_USB_FULL_SPEED_OPEN	    1
#define QUEC_FLAG_DATA_LEN              2
#define QUECTEL_USB_SPEED_CLRT          11

typedef QUEC_PACKED struct
{
	raw_data_type items[RAW_DATA_ITEM_NUMS];
	uint32_t	crc;
} QUEC_PACKED_POST raw_data_list_type;

typedef struct
{
	char data_cache[RAW_DATA_CACHE_SIZE];
	bool data_dirty;
} raw_data_cache_type;

extern const uint32_t quec_raw_data_item_len_list[];

static raw_data_cache_type* raw_data_cache = NULL;

static flash_handle_t raw_flash_handle = NULL;

static DEFINE_MUTEX(rawdata_lock);

atomic_t client_count = {0};

static int get_start_block_id(flash_handle_t handle)
{
	return 0;
}

static int get_next_block_id(int block_id)
{
	return (block_id + 1);
}

static bool verify_raw_config_data(raw_data_list_type* raw_data_list)
{
	uint8_t* data = (uint8_t*) raw_data_list->items;

	if (crc_32_calc(data, sizeof(raw_data_list->items) * 8, 0) 
			!= raw_data_list->crc)
	{
		return false;
	}

	return true;
}

static bool load_raw_data_list(void)
{
	int block_id;
	int i, j;
	char* cache_ptr = NULL;
	page_id page;
	raw_data_list_type* raw_data_list;
	uint32_t page_nums;
	uint32_t page_size;
	int data_len = sizeof(raw_data_list_type);
	char* page_buf;
	
	if (raw_flash_handle == NULL)
	{
		return false;
	}

	if (raw_data_cache == NULL)
	{
		raw_data_cache = (raw_data_cache_type*)kmalloc(sizeof(raw_data_cache_type), GFP_KERNEL);
		if (raw_data_cache == NULL)
		{
			return false;
		}
		memset(raw_data_cache, 0, sizeof(raw_data_list_type));
	}

	page_nums = quec_flash_pages_per_block(raw_flash_handle);
	page_size = quec_flash_page_size(raw_flash_handle);
	data_len = sizeof(raw_data_list_type);
	raw_data_list = (raw_data_list_type*) raw_data_cache->data_cache;
	
	memset(raw_data_list, 0, sizeof(raw_data_list_type));
	
	block_id = get_start_block_id(raw_flash_handle);
	page_buf = (char*) kmalloc(page_size, GFP_KERNEL);
	if (page_buf == NULL)
	{
		return false;
	}
	
	for (i=0; i<RAW_FLASH_DATA_BLOCK_NUMS; i++)
	{ 	
		if (quec_flash_is_block_good(raw_flash_handle, block_id) == false)
		{
			block_id = get_next_block_id(block_id);
			continue;
		}

		page = quec_flash_get_page_id(raw_flash_handle, block_id, 0);
		
		memset(raw_data_cache->data_cache, 0, sizeof(raw_data_cache->data_cache));
		cache_ptr = raw_data_cache->data_cache;

		data_len = sizeof(raw_data_list_type);
		
		for (j=0; j<page_nums; j++)
		{
			int copy_len = page_size > data_len ? data_len : page_size;
			
			if (quec_flash_read_page(raw_flash_handle, page+j, page_buf) != FLASH_DEVICE_DONE)
			{
				break;
			}

			memcpy(cache_ptr, page_buf, copy_len);
			cache_ptr += copy_len;
			data_len -= copy_len;
			if (data_len <= 0)
				break;
		}

		if ((data_len <= 0) && verify_raw_config_data(raw_data_list))
		{
			kfree(page_buf);
			return true;
		}

		block_id = get_next_block_id(block_id);
	}

	kfree(page_buf);
	return false;;
}

bool quec_raw_flash_data_open(void)
{
	bool ret = false;

	mutex_lock(&rawdata_lock);
	
	if (sizeof(raw_data_list_type) > sizeof(raw_data_cache->data_cache))
	{
		goto out;
	}

	if (RAW_DATA_MAX_ID >= RAW_DATA_ITEM_NUMS)
	{
		goto out;
	}
	
	if (raw_flash_handle)
	{
		ret = true;
		atomic_inc(&client_count);
		goto out;
	}

	raw_flash_handle = quec_flash_open((unsigned char*)QUECTEL_RAW_DATA_PARTITION_NAME);
	if (raw_flash_handle == NULL)
	{
		goto out;
	}
	
	if (load_raw_data_list() == false)
	{
		raw_data_list_type* raw_data_list = (raw_data_list_type*) raw_data_cache->data_cache;
		memset(raw_data_list, 0, sizeof(raw_data_list_type));
	}
	
	raw_data_cache->data_dirty = false;
	ret = true;
	atomic_inc(&client_count);

out:
	mutex_unlock(&rawdata_lock);
	return ret;
}

bool raw_flash_data_sync(bool lock_mutex)
{
	int block_id;
	int i, j;
	page_id page;
	raw_data_list_type* raw_data_list;
	bool ret = false;
	char* cache_ptr = NULL;
	uint32_t page_nums;
	uint32_t page_size;
	int data_len;
	int err;
	
	if (lock_mutex)
	{
		mutex_lock(&rawdata_lock);
	}

	if (raw_flash_handle == NULL)
	{
		goto out;
	}

	page_nums = quec_flash_pages_per_block(raw_flash_handle);
	page_size = quec_flash_page_size(raw_flash_handle);
	data_len = sizeof(raw_data_list_type);
	raw_data_list = (raw_data_list_type*) raw_data_cache->data_cache;
	
	if (raw_data_cache->data_dirty == false)
	{
		ret = true;
		goto out;
	}

	raw_data_list->crc = crc_32_calc((uint8_t*)raw_data_list->items, sizeof(raw_data_list->items) * 8, 0);

	block_id =	get_start_block_id(raw_flash_handle);

	for (i=0; i<RAW_FLASH_DATA_BLOCK_NUMS; i++)
	{
		if (quec_flash_is_block_good(raw_flash_handle, block_id) == false)
		{
			block_id = get_next_block_id(block_id);
			
			continue;
		}

		err = quec_flash_erase_block(raw_flash_handle, block_id);
		if (err != FS_DEVICE_OK)
		{
			if (err == -EIO)
			{
				quec_flash_set_block_state(raw_flash_handle, block_id, FLASH_BLOCK_BAD);
			}
			block_id = get_next_block_id(block_id);

			continue;
		}

		page = quec_flash_get_page_id(raw_flash_handle, block_id, 0);
		cache_ptr = raw_data_cache->data_cache;

		data_len = sizeof(raw_data_list_type);
		for (j=0; j<page_nums; j++)
		{
			int copy_len = page_size > data_len ? data_len : page_size;

			if (quec_flash_write_page(raw_flash_handle, page+j, (void*)cache_ptr) != FLASH_DEVICE_DONE)
			{
				break;
			}

			cache_ptr += copy_len;
			data_len -= copy_len;

			if (data_len <= 0)
			{
				ret = true;
				raw_data_cache->data_dirty = false;
				break;
			}
		}
		
		block_id = get_next_block_id(block_id);
	}

out:
	if (lock_mutex)
	{
		mutex_unlock(&rawdata_lock);
	}
	return ret;
}

bool quec_raw_flash_data_sync(void)
{
	return raw_flash_data_sync(true);
}

bool quec_raw_flash_data_close(void)
{
	bool ret = false;
	
	mutex_lock(&rawdata_lock);

	if (raw_flash_handle == NULL)
	{
		goto out;
	}

	if (atomic_read(&client_count) > 0)
		atomic_dec(&client_count);
	
	if (atomic_read(&client_count) > 0)
	{
		ret = true;
		raw_flash_data_sync(false);
		goto out;
	}

	raw_flash_data_sync(false);

	if (quec_flash_close(raw_flash_handle) == FLASH_DEVICE_DONE)
	{
		raw_flash_handle = NULL;
		kfree(raw_data_cache);
		raw_data_cache = NULL;
		ret = true;
		goto out;
	}

out:
	mutex_unlock(&rawdata_lock);

	return ret;
}

bool quec_raw_flash_data_write_item(quec_raw_data_id data_id, void* data)
{
	raw_data_list_type* raw_data_list;
	bool ret = false;
	
	mutex_lock(&rawdata_lock);
	
	if (raw_data_cache == NULL || raw_flash_handle == NULL)
	{
		goto out;
	}

	raw_data_list = (raw_data_list_type*) raw_data_cache->data_cache;
	
	if (data_id >= RAW_DATA_MAX_ID)
	{
		goto out;
	}
	if (quec_raw_data_item_len_list[data_id] == 0)
	{
		goto out;
	}
	
	if (((uint32_t)raw_data_list->items[data_id].data_len) == quec_raw_data_item_len_list[data_id]
	    && memcmp(raw_data_list->items[data_id].data, data, quec_raw_data_item_len_list[data_id]) == 0)		
	{
		ret = true;
		goto out;
	}

	memset(raw_data_list->items[data_id].data, 0, sizeof(raw_data_list->items[data_id].data));
	memcpy(raw_data_list->items[data_id].data, data, quec_raw_data_item_len_list[data_id]);
	raw_data_list->items[data_id].data_len = quec_raw_data_item_len_list[data_id];

	raw_data_cache->data_dirty = true;

	ret = true;

out:
	mutex_unlock(&rawdata_lock);

	return ret;
}

bool quec_raw_flash_data_read_item(quec_raw_data_id data_id, void* data)
{
	raw_data_list_type* raw_data_list;
	bool ret = false;
	
	mutex_lock(&rawdata_lock);
	
	if (raw_data_cache == NULL || raw_flash_handle == NULL)
	{
		goto out;
	}
	
	raw_data_list = (raw_data_list_type*) raw_data_cache->data_cache;

	if (data_id >= RAW_DATA_MAX_ID)
	{
		goto out;
	}

	if (quec_raw_data_item_len_list[data_id] == 0)
	{
		goto out;
	}

	if (raw_data_list->items[data_id].data_len != quec_raw_data_item_len_list[data_id])
	{
		goto out;
	}

	memcpy(data, raw_data_list->items[data_id].data, quec_raw_data_item_len_list[data_id]);
	ret = true;

out:
	
	mutex_unlock(&rawdata_lock);
	return ret;
}

static int rawdata_open(struct inode *inode, struct file *file) {
	quec_raw_flash_data_open();
	return 0;
}

static int rawdata_release(struct inode *inode, struct file *file) {
	quec_raw_flash_data_close();
	return 0;
}

static long rawdata_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	struct rawdata_item item;
	int error = 0;
	
	switch (cmd) {
	case QUEC_IOCTL_RAWDATA_GET_ITEM:
		if (copy_from_user(&item, (struct rawdata_item __user *) arg,
				 sizeof(struct rawdata_item))) {
			error = -EFAULT;
			break;
		}

		if ((item.id < 0) || (item.id >= RAW_DATA_MAX_ID)) {
			error = -EFAULT;
			break;
		}

		if (!quec_raw_flash_data_open()) {
			error = -EFAULT;
			break;
		}
		item.value.data_len = quec_raw_data_item_len_list[item.id];

		if (quec_raw_flash_data_read_item(item.id, item.value.data)) {
			if (copy_to_user((void __user *)arg, &item, sizeof(struct rawdata_item))) {
				error = -EFAULT;
			}
		} else {
			error = -EFAULT;
		}
		quec_raw_flash_data_close();
		break;
		
	case QUEC_IOCTL_RAWDATA_SET_ITEM:
		if (copy_from_user(&item, (struct rawdata_item __user *) arg,
				 sizeof(struct rawdata_item))) {
			error = -EFAULT;
			break;
		}

		if ((item.id < 0) || (item.id >= RAW_DATA_MAX_ID)) {
			error = -EFAULT;
			break;
		}
		if (item.value.data_len != quec_raw_data_item_len_list[item.id]) {
			error = -EFAULT;
			break;
		}
			
		if (!quec_raw_flash_data_open()) {
			error = -EFAULT;
			break;
		}

		if (!quec_raw_flash_data_write_item(item.id, item.value.data)) {
			error = -EFAULT;
		}
		
		quec_raw_flash_data_close();
		break;
	}

	return error;
}

#if 1/*Gavin.lu 20231124 Add usb boot From Kernel,here Get usbcfg from rowdata */
int quec_rawdata_read_usb_cfg(struct rawdata_item *item_usb1)
{
	int error = 0;

	if (!quec_raw_flash_data_open()) {
		printk("[quectel] quec rawdata open fail \r\n");
		error = -EFAULT;
		return error;
	}

	item_usb1->id = RAW_DATA_SYSTEM_CFG; 
	item_usb1->value.data_len = quec_raw_data_item_len_list[item_usb1->id];
	if (!quec_raw_flash_data_read_item(item_usb1->id, item_usb1->value.data)) {
			printk("[quectel] quec rawdata read usb1cfg fail \r\n");
			error = -EFAULT;
			goto QError;
	}
QError:	
		quec_raw_flash_data_close();

	return error;
}

EXPORT_SYMBOL_GPL(quec_rawdata_read_usb_cfg);
#endif

/* Zeke 20250424 Add use usb full speed mode */
int quec_rawdata_read_usb_full_speed(void)
{
	ql_rawdata_flag_ctl_type quec_rawdata_data;
	char buf[RAWDATA_MANAGE_LEN] = {};
	char flag_sum = 0;
	int i = 0;
	int result = 0;
	
	if (!quec_raw_flash_data_open()) {
		printk("[quectel] quec rawdata open fail \r\n");
		result = -EFAULT;
		return result;
	}
	
	memset(&quec_rawdata_data,0,sizeof(ql_rawdata_flag_ctl_type));
	
	if (!quec_raw_flash_data_read_item(RAW_DATA_FLAG_CTL_CFG, &quec_rawdata_data)) {
			printk("[quectel] quec rawdata read usbcfg fail \r\n");
			result = -EFAULT;
			goto QError;
	}
	
	flag_sum = quec_rawdata_data.flag_sum;
	memcpy(buf,quec_rawdata_data.data,quec_rawdata_data.used_total_len);
	
	if(((int)quec_rawdata_data.used_total_len == (int)(flag_sum*QUEC_FLAG_DATA_LEN)) && flag_sum > 0 && (char)QUECTEL_USB_SPEED_CLRT <= flag_sum){
		while(flag_sum != 0){
			if(buf[i] == QUECTEL_USB_SPEED_CLRT){
				if((buf[i+1] == QUECTEL_USB_FULL_SPEED_OPEN) || (buf[i+1] == QUECTEL_USB_FULL_SPEED_CLOSE)){
					result = buf[i+1];
				}
				else
					printk("[quectel] Invalid USB speed mode and buf=%d!!!\n",  buf[i+1]);
				break;
			}	
			else{
				i+=QUEC_FLAG_DATA_LEN;
				flag_sum --;
			}
		}
	}
QError:	
	quec_raw_flash_data_close();
	return result;
}

EXPORT_SYMBOL_GPL(quec_rawdata_read_usb_full_speed);

static const struct file_operations rawdata_fops = {
	.owner = THIS_MODULE,
	.open = rawdata_open,
	.unlocked_ioctl = rawdata_ioctl,
	.llseek = NULL,
	.release = rawdata_release,
};

static int __init quec_rawdata_proc_init(void)
{
	proc_create("quec_rawdata", 0, NULL, &rawdata_fops);
	return 0;
}

__initcall(quec_rawdata_proc_init);

