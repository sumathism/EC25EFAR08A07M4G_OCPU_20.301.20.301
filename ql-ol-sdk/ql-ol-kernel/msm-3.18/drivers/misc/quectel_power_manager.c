/* Copyright (c) 2010-2011, The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
/*
 * Quectel power_manager  driver
 *
 */
/*===========================================================================================


					EDIT HISTORY FOR MODULE

	This section contains comments describing changes made to the module.
	Notice that changes are listed in reverse chronological order.

 WHEN		WHO		WHAT,WHERE,WHY
 ----------     -----------     ------------------------------------------------------------
 25/12/2018     WW		Disable wakeupin to make module go in sleep.
 17/07/2019     WW		Modify to get wkaeup in level only when wakeup in is enable.
				And enable MINIPCIE_MODE_PIN.
===========================================================================================*/

#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/err.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <asm/atomic.h>
#include <linux/init.h>
#include <linux/gpio.h>
#include <linux/spinlock.h>
#include <linux/quectel_power_manager.h>

#include <linux/of.h>
#include <linux/of_gpio.h>

#define DTR_ESCAPE_MODE

#ifdef DTR_ESCAPE_MODE//francis
#include <soc/qcom/smd.h>
#endif
/* Private data */

#define QUECTEL_OPEN_SLEEP_IND_FEATURE/*duke@markfalg20180609 open sleep indicate feature */
#define MINIPCIE_MODE_PIN/*duke@markfalg20180627 open wake up in   feature */


#ifdef MINIPCIE_MODE_PIN
static DEFINE_MUTEX(wakeup_lock);
#endif

static DEFINE_MUTEX(dtr_lock);
/* Internal functions */

#ifdef DTR_ESCAPE_MODE

#define	DTR_SMD_CHANNEL_NAME "DATA3"

static DEFINE_MUTEX(dtr_ch_lock);

struct dtr_device_t{
    struct smd_channel *ch;
    char name[10];
    int flag;//0 dtr low 1:dtr high
    int smd_open;
};

struct dtr_device_t *dtr_devp = NULL;

struct dtr_device_t dtr_device = {
    .name = "dtr", 
    .flag = -1,	
};

#endif//DTR_ESCAPE_MODE

#ifdef MINIPCIE_MODE_PIN
//lory for sleep ind
static int sleep_pola_ind = 0;
static int wakeup_in_active_level = 0;
#if 1/*add duke20180629*/
static int wakeup_enable_value = 0;/*deflaut open it*/
#endif
static ssize_t quectel_sleep_pola_ind_show
(
	struct device *pdev,
	struct device_attribute *attr,
	char *buf
)
{
      printk("[lory][sleepin] %s: irq\n", __func__);
	return snprintf(buf, PAGE_SIZE, "%d\n", sleep_pola_ind);
}

static ssize_t quectel_sleep_pola_ind_store
(
	struct device *pdev,
	struct device_attribute *attr,
	const char *buff, size_t size
)
{
	int enable;
	
	sscanf(buff, "%d", &enable);

      printk("[lory][sleepin] sleepind store %d\n", enable);
	if(enable == 0 || enable == 1)
	{
		sleep_pola_ind = enable;
	}
	else
	{
		pr_err("sleep_pola_ind out of range.\n");
	}
	
	return size;
}


static ssize_t wakeup_in_active_level_show
(
	struct device *pdev,
	struct device_attribute *attr,
	char *buf
)
{
	return snprintf(buf, PAGE_SIZE, "%d\n", wakeup_in_active_level);
}

static ssize_t wakeup_in_active_level_store
(
	struct device *pdev,
	struct device_attribute *attr,
	const char *buff, size_t size
)
{
	int enable;
	
	sscanf(buff, "%d", &enable);

	if(enable == 0 || enable == 1)
	{
		wakeup_in_active_level = enable;
	}
	else
	{
		pr_err("wakeup_in_active_level out of range.\n");
	}
	
	return size;
}

//lory end
#elif defined(QUECTEL_OPEN_SLEEP_IND_FEATURE)

static int sleep_pola_ind = 0;
static ssize_t quectel_sleep_pola_ind_show
(
	struct device *pdev,
	struct device_attribute *attr,
	char *buf
)
{
      printk("[lory][sleepin] %s: irq\n", __func__);
	return snprintf(buf, PAGE_SIZE, "%d\n", sleep_pola_ind);
}

static ssize_t quectel_sleep_pola_ind_store
(
	struct device *pdev,
	struct device_attribute *attr,
	const char *buff, size_t size
)
{
	int enable;
	
	sscanf(buff, "%d", &enable);

      printk("[lory][sleepin] sleepind store %d\n", enable);
	if(enable == 0 || enable == 1)
	{
		sleep_pola_ind = enable;
	}
	else
	{
		pr_err("sleep_pola_ind out of range.\n");
	}
	
	return size;
}

#endif

#ifdef MINIPCIE_MODE_PIN
static irqreturn_t quectel_wakeup_irq(int irq, void *d)
{
	struct quectel_power_manager *data = (struct quectel_power_manager *)d;

	atomic_set(&data->notify_wakeup_in, 1);
	schedule_delayed_work(&data->sysfs_notify_work, 0);
	wake_lock(&data->wlock_wakeup);
	if (data->wakeup_flag)
	{
		printk("[Max][DTR] %s: irq\n", __func__);
	}

	//printk("[duke][WAKE]  [%s:%d] \n", __FUNCTION__, wakeup_enable_value);

	return IRQ_HANDLED;
}
#endif

static irqreturn_t quectel_dtr_irq(int irq, void *d)
{
	struct quectel_power_manager *data = (struct quectel_power_manager *)d;

	atomic_set(&data->notify_dtr_in, 1);
	atomic_set(&data->notify_dtr_state, 1);
	schedule_delayed_work(&data->sysfs_notify_work, 0);
	schedule_delayed_work(&data->dtr_smd_work, 0);
	wake_lock(&data->wlock_dtr);
	if (data->dtr_flag)
	{
		printk("[Max][DTR] %s: irq\n", __func__);
	}
	return IRQ_HANDLED;
}

#ifdef MINIPCIE_MODE_PIN
static int quectel_wakeup_init(struct quectel_power_manager *data)
{
	struct quectel_power_manager_pdata *pdata = data->pdata;
	int err;
	
	err = gpio_request(pdata->gpio_wakeup, "wakeup_in");
	if (err < 0)
	{
		printk("[Max][DTR] %s: request gpio: %d error: %d\n", __func__, pdata->gpio_wakeup, err);
		goto err_gpio_request;
	}
	
	err = gpio_direction_input(pdata->gpio_wakeup);
	if (err < 0)
	{
		printk("[Max][DTR] %s: set gpio direction input (%d) fail\n", __func__, pdata->gpio_wakeup);
		goto err_gpio_to_irq;
	}

	err = gpio_to_irq(pdata->gpio_wakeup);
	if (err < 0)
	{
		printk("[Max][DTR] %s: gpio to irq: %d error: %d\n", __func__, pdata->gpio_wakeup, err);
		goto err_gpio_to_irq;
	}
	
	data->irq_wakeup = err;
    
	printk("[lory][wakeupin] %s\n", "wakeup trigger to double-edge-triggered");
	err = request_any_context_irq(data->irq_wakeup, quectel_wakeup_irq, \
			IRQF_TRIGGER_FALLING|IRQF_TRIGGER_RISING, "quectel_wakeup_event", data); //lory 20160922,???????????|
	if (err < 0)
	{
		printk("[Max][DTR] %s: Can't get %d IRQ for wakeup_in: %d\n", __func__, data->irq_wakeup, err);
		goto err_gpio_to_irq;
	}

	//disable_irq(data->irq_wakeup); //because use edge trigger mode, so can enable this irq

	return 0;

err_gpio_to_irq:
	gpio_free(pdata->gpio_wakeup);
err_gpio_request:
	return err;
}
#endif

#ifdef DTR_ESCAPE_MODE

void send_data_to_smd(int value)
{
  unsigned cbits_to_modem;
  
  if(value)//dtr high
  {
     cbits_to_modem = 0;
  }
  else
  {
     cbits_to_modem = 2;
  }
  
  if(dtr_devp->flag == value)
  {
    //
  }
  else
  {
    printk("[Max][DTR] dtr_devp->flag=%d,value = %d,cbits_to_modem = %d,~cbits_to_modem = %d func = %s\n",dtr_devp->flag,value,cbits_to_modem,~cbits_to_modem,__func__);

	dtr_devp->flag = value;
    smd_tiocmset(dtr_devp->ch,cbits_to_modem,~cbits_to_modem);
  }
}

static void dtr_notify(void *priv, unsigned event)
{
	printk("[Max][DTR]  [%s:%d] event=%d\n", __FUNCTION__, __LINE__,event);
  switch(event)
  {
    case SMD_EVENT_DATA:
		printk("[Max][DTR] DTR SMD EVENT DATA\n");
		break;

    case SMD_EVENT_OPEN:
		dtr_devp->smd_open = 1;
		printk("[Max][DTR] DTR SMD EVENT OPEN\n");
		break;
  
    case SMD_EVENT_CLOSE:
		printk("[Max][DTR] DTR SMD EVENT CLOSE\n");
		break;
  }

}

static int dtr_smd_open( void )
{
  int r = 0;
 
  mutex_lock(&dtr_ch_lock);
  dtr_devp = &dtr_device;
 
  if(dtr_devp->ch == 0)
  {
    r = smd_named_open_on_edge(DTR_SMD_CHANNEL_NAME,SMD_APPS_MODEM,&dtr_devp->ch,dtr_devp,dtr_notify);
    if (r == 0 && dtr_devp->smd_open == 0) {
        int smd_open_timeout = 10;
        while (dtr_devp->smd_open == 0 && smd_open_timeout-- > 0) {
            msleep(10);
        }
    }
    printk("[Max][DTR][%s:%d] smd_open_ret = %d, smd_open = %d\n", __FUNCTION__, __LINE__, r, dtr_devp->smd_open);
  }
  mutex_unlock(&dtr_ch_lock);
  return r;
}

static int dtr_smd_release( void )
{
  int r = 0;
  mutex_lock(&dtr_ch_lock);
  if(dtr_devp->ch != 0)
  {
    r = smd_close(dtr_devp->ch);
	dtr_devp->ch = 0;
  }
  mutex_unlock(&dtr_ch_lock);
  return r;
}

static void quectel_dtr_smd_work(struct work_struct *work) {
    struct quectel_power_manager *data = container_of(work, struct quectel_power_manager, dtr_smd_work.work);
    struct quectel_power_manager_pdata *pdata = data->pdata;

    //open smd data3
    if (!dtr_smd_open()) {
        mutex_lock(&dtr_lock);
        send_data_to_smd(!!gpio_get_value(pdata->gpio_dtr));
        mutex_unlock(&dtr_lock);
    } else {
        schedule_delayed_work(&data->dtr_smd_work, HZ);
    }
}
#endif//DTR_ESCAPE_MODE

static int quectel_dtr_init(struct quectel_power_manager *data)
{
	struct quectel_power_manager_pdata *pdata = data->pdata;
	int err;
#if 0
#ifdef DTR_ESCAPE_MODE//francis
	//open smd data5
	dtr_smd_open();
#endif
#endif
	err = gpio_request(pdata->gpio_dtr, "dtr_in");
	if (err < 0)
	{
		printk("[Max][DTR] %s: request gpio: %d error: %d\n", __func__, pdata->gpio_dtr, err);
		goto err_gpio_request;
	}

	err = gpio_direction_input(pdata->gpio_dtr);
	if (err < 0)
	{
		printk("[Max][DTR] %s: set gpio direction input (%d) fail\n", __func__, pdata->gpio_dtr);
		goto err_gpio_to_irq;
	}

	err = gpio_to_irq(pdata->gpio_dtr);
	if (err < 0)
	{
		printk("[Max][DTR] %s: gpio to irq: %d error: %d\n", __func__, pdata->gpio_dtr, err);
		goto err_gpio_to_irq;
	}
	data->irq_dtr = err;

	err = request_any_context_irq(data->irq_dtr, quectel_dtr_irq, \
			IRQF_TRIGGER_FALLING|IRQF_TRIGGER_RISING, "quectel_dtr_event", data);
	if (err < 0)
	{
		printk("[Max][DTR] %s: Can't get %d IRQ for dtr_in: %d\n", __func__, data->irq_dtr, err);
		goto err_gpio_to_irq;
	}

	//disable_irq(data->irq_dtr);

#if 0
#ifdef DTR_ESCAPE_MODE//francis
	//open smd data5
	dtr_smd_open();
#endif
#endif
	return 0;

err_gpio_to_irq:
	gpio_free(pdata->gpio_dtr);
err_gpio_request:
	return err;
}

static int quectel_sleep_state_init(struct quectel_power_manager *data)
{
	struct quectel_power_manager_pdata *pdata = data->pdata;
	int err;
	printk("[lory][sleepind] %s\n", "sleep ind init ");
	err = gpio_request(pdata->gpio_sleep_state, "sleep_state_out");
	if (err < 0)
	{
		printk("[Max][DTR] %s: request gpio: %d error: %d\n", __func__, pdata->gpio_sleep_state, err);
		goto err_gpio_request;
	}

	if (gpio_direction_output(pdata->gpio_sleep_state, 0))
	{
		printk("[Max][DTR] %s: set gpio direction output err\n", __func__);
		goto err_gpio_direction_output;
	}

	return 0;

err_gpio_direction_output:
	gpio_free(pdata->gpio_sleep_state);
err_gpio_request:
	return err;
}

#ifdef MINIPCIE_MODE_PIN
static ssize_t quectel_wakeup_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct quectel_power_manager *data = dev_get_drvdata(dev);
	struct quectel_power_manager_pdata *pdata = data->pdata;
	int val;
	ssize_t status;
	
	mutex_lock(&wakeup_lock);

	val = !!gpio_get_value(pdata->gpio_wakeup);
	status = sprintf(buf, "%d\n", val);
	dev_info(dev, "%s %s", __func__, buf);
	wake_unlock(&data->wlock_wakeup);

	mutex_unlock(&wakeup_lock);
	
	return status;
}
#endif

static ssize_t quectel_dtr_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct quectel_power_manager *data = dev_get_drvdata(dev);
	struct quectel_power_manager_pdata *pdata = data->pdata;
	int val;
	ssize_t status;

	mutex_lock(&dtr_lock);

	val = !!gpio_get_value(pdata->gpio_dtr);
	status = sprintf(buf, "%d\n", val);
	dev_info(dev, "%s %s", __func__, buf);
	wake_unlock(&data->wlock_dtr);

	mutex_unlock(&dtr_lock);
	
	return status;
}

static void quectel_sysfs_notify_work(struct work_struct *work) {
    struct quectel_power_manager *data = container_of(work, struct quectel_power_manager, sysfs_notify_work.work);

#ifdef MINIPCIE_MODE_PIN
    if (atomic_read(&data->notify_wakeup_in)) {
        atomic_set(&data->notify_wakeup_in, 0);
        sysfs_notify(&data->pdev->dev.kobj, NULL, "wakeup_in");
        if (gpio_get_value(data->pdata->gpio_wakeup) != wakeup_in_active_level) {
            wake_unlock(&data->wlock_wakeup);
			 ///wake_unlock(&data->wlock_dtr);
        }
		//printk("[duke][DTR]  [sss%s:%d] \n", __FUNCTION__, wakeup_enable_value);
    }
	///printk("[duke][DTR]  [%s:%d] \n", __FUNCTION__, wakeup_enable_value);
#endif

    if (atomic_read(&data->notify_dtr_in)) {
        atomic_set(&data->notify_dtr_in, 0);
        sysfs_notify(&data->pdev->dev.kobj, NULL, "dtr_in");
        if (gpio_get_value(data->pdata->gpio_dtr)) {
            wake_unlock(&data->wlock_dtr);
        }
    }
    
    if (atomic_read(&data->notify_dtr_state)) {
        atomic_set(&data->notify_dtr_state, 0);
        sysfs_notify(&data->pdev->dev.kobj, NULL, "dtr_state");
        if (gpio_get_value(data->pdata->gpio_dtr)) {
            wake_unlock(&data->wlock_dtr);
        }
    }
}

static ssize_t quectel_dtr_state_show(struct device *dev, struct device_attribute *attr, char *buf) {
	struct quectel_power_manager *data = dev_get_drvdata(dev);
	struct quectel_power_manager_pdata *pdata = data->pdata;

	if (!strcmp(attr->attr.name, "dtr_state")) {
		return sprintf(buf, "%d\n", !!gpio_get_value(pdata->gpio_dtr));
	} else {
		return 0;
	}
}
static DEVICE_ATTR(dtr_state, 0444, quectel_dtr_state_show, NULL);
static DEVICE_ATTR(dtr_in, 0444, quectel_dtr_show, NULL);

#ifdef MINIPCIE_MODE_PIN
#if 1/*add duke20180629*/
static ssize_t wakeup_enable_show
(
	struct device *pdev,
	struct device_attribute *attr,
	char *buf
)
{
	return snprintf(buf, PAGE_SIZE, "%d\n", wakeup_enable_value);
}

static ssize_t wakeup_enable_store
(
	struct device *pdev,
	struct device_attribute *attr,
	const char *buff, size_t size
)
{
	int enable;
	static char flag=0;
	struct quectel_power_manager *data = dev_get_drvdata(pdev);
	//struct quectel_power_manager_pdata *pdata = data->pdata;

	sscanf(buff, "%d", &enable);

	if(enable == 0 || enable == 1)
	{
		if(wakeup_enable_value != enable)
		{
			wakeup_enable_value = enable;
		}
		else 
		{
			printk("[Duke][Wake] %s: Same config \n", __func__);
			goto ERROR;
		}
	}
	else
	{
		pr_err("wakeup_enable out of range.\n");
		goto ERROR;
	}

	if(wakeup_enable_value==0)
	{
#if 0/**/
		disable_irq(data->irq_wakeup);
		if (disable_irq_wake(data->irq_wakeup) < 0)
		{
			printk("[Duke][WAKE] %s: disable irq wake fail\n", __func__);
		}
#endif		
		printk("[duke][WAKE]  [-nothing to do-%s:%d] \n", __FUNCTION__, wakeup_enable_value);

	}
	else 
	{
		if(flag==1)
		{
			printk("[Duke][Wake] %s:  Just init one return error \n", __func__);
			goto ERROR;
		}
		if(quectel_wakeup_init(data) < 0)
		{
			printk("[Duke][Wake] %s: init wakeup gpio fail\n", __func__);
			goto ERROR;
		}
		flag=1;
#if 0	
		enable_irq(data->irq_wakeup);
		if (enable_irq_wake(data->irq_wakeup) < 0)
		{
			printk("[Max][DTR] %s: enable irq wake fail\n", __func__);
		}
		else
		{
			data->wakeup_flag = 1;
		}
#endif		
	}
		printk("[duke][WAKE]  [%s:%d] \n", __FUNCTION__, wakeup_enable_value);
ERROR:
		
	return size;
}
static DEVICE_ATTR(wakeup_enable, 0664, wakeup_enable_show, wakeup_enable_store); //duke 2016/06/29
#endif
static DEVICE_ATTR(wakeup_in, 0444, quectel_wakeup_show, NULL);
static DEVICE_ATTR(sleep_polarity, 0664, quectel_sleep_pola_ind_show, quectel_sleep_pola_ind_store); //lory 2016/09/21
static DEVICE_ATTR(wakeup_in_polarity, 0664, wakeup_in_active_level_show, wakeup_in_active_level_store); //lory 2016/09/21
#elif defined(QUECTEL_OPEN_SLEEP_IND_FEATURE)
static DEVICE_ATTR(sleep_polarity, 0664, quectel_sleep_pola_ind_show, quectel_sleep_pola_ind_store); //lory 2016/09/21

#endif

#define GPIO_NAME_MAX_LEN 32
static int quectel_dt_parse_gpio_info(struct device *dev,
		struct quectel_power_manager_of_data *of_data)
{
	int ret = 0, cnt, i, alternative_ind_gpio = -1;
	struct device_node *np = dev->of_node;
	
	of_property_read_u32(np, "aternative_sleep_ind_gpio", &alternative_ind_gpio);

	cnt = of_gpio_count(np);
	if (cnt > 0) {
		of_data->size = cnt;
		of_data->gpio = devm_kzalloc(dev, cnt *
				sizeof(struct quectel_power_manager_of_data), GFP_KERNEL);

		if (!of_data->gpio) {
			dev_err(dev, "No memory for gpio\n");
			ret = -ENOMEM;
			goto out;
		}

		for (i = 0; i < cnt; i++) {
			const char *name = NULL;
			char *result = NULL;

			of_data->gpio[i].no = of_get_gpio(np, i);
			of_property_read_string_index(np,
					"quec,gpio-names", i, &name);
			result = (char*)devm_kzalloc(dev, strlen(name) + 1, GFP_KERNEL);

			snprintf(result, strlen(name) + 1, "%s",
					name);
			of_data->gpio[i].name = result;
			if(!strcmp(of_data->gpio[i].name, "sleep_ind") && alternative_ind_gpio >= 0 && alternative_ind_gpio <= 91)
				of_data->gpio[i].no = alternative_ind_gpio;
			printk("[Max][DTR] %s: cnt: %d, gpio[%s] = %d\n", __func__, i, 
				of_data->gpio[i].name,
				of_data->gpio[i].no);
		}
	}

out:
	if (ret)
		dev_err(dev, "%s failed with err %d\n", __func__, ret);
	return ret;
}

static int quectel_power_manager_probe(struct platform_device *pdev)
{
	struct quectel_power_manager *data = NULL;
	struct quectel_power_manager_of_data *of_data = NULL;
	struct quectel_power_manager_pdata *pdata =NULL;
	struct device *dev = &(pdev->dev);
	int i = 0;
	
	printk("[Max][DTR]  [%s:%d] \n", __FUNCTION__, __LINE__);
	of_data = devm_kzalloc(dev, sizeof(struct quectel_power_manager_of_data), GFP_KERNEL);
	if (of_data == NULL) {
		printk("[Max][DTR] %s:kzalloc() of_data failed.\n", __func__);
		return -ENOMEM;
	}
	
	if(quectel_dt_parse_gpio_info(dev, of_data))
	{
		printk("[Max][DTR] %s:kzalloc() failed.\n", __func__);
		return -ENOMEM;
	}

	pdata = devm_kzalloc(dev, sizeof(struct quectel_power_manager_pdata), GFP_KERNEL);
	if (pdata == NULL) {
		printk("[Max][DTR] %s:kzalloc() failed.\n", __func__);
		return -ENOMEM;
	}

	for(i = 0; i < of_data->size; i++)
	{
		if(!strncmp("wakeup_in", of_data->gpio[i].name, strlen("wakeup_in")))
		{	
			pdata->gpio_wakeup = of_data->gpio[i].no;		
			printk("[Max][DTR] %s: find wakeup_in: %d \n", __func__, pdata->gpio_wakeup);
		}
		
		if(!strncmp("sleep_ind", of_data->gpio[i].name, strlen("sleep_ind")))
		{
			pdata->gpio_sleep_state = of_data->gpio[i].no;		
			printk("[Max][DTR] %s: find sleep_ind: %d \n", __func__, pdata->gpio_sleep_state);
		}
		
		if(!strncmp("dtr", of_data->gpio[i].name, strlen("dtr")))
		{
			pdata->gpio_dtr= of_data->gpio[i].no;		
			printk("[Max][DTR] %s: find dtr: %d \n", __func__, pdata->gpio_dtr);
		}
		/******************************************************************************************
		asa.wang-2018/04/02:when the system goes to sleep, turn off the net status indicator
		Refer to [Issue-Depot].[IS0000099][Submitter:asa.wang,Date:2018-04-02][add gpio_net_status]
		******************************************************************************************/
		if(!strncmp("net_status", of_data->gpio[i].name, strlen("net_status")))
		{
			pdata->gpio_net_status= of_data->gpio[i].no;		
			printk("[MAX][DTR] %s: find dtr: %d \n", __func__, pdata->gpio_net_status);
		}
	}

	pdata->of_data = of_data;

	data = devm_kzalloc(dev, sizeof(struct quectel_power_manager), GFP_KERNEL);
	if (data == NULL) {
		printk("[Max][DTR] %s:kzalloc() failed.\n", __func__);
		return -ENOMEM;
	}

	INIT_DELAYED_WORK(&data->sysfs_notify_work, quectel_sysfs_notify_work);
	INIT_DELAYED_WORK(&data->dtr_smd_work, quectel_dtr_smd_work);

	atomic_set(&data->notify_wakeup_in, 0);
	atomic_set(&data->notify_dtr_in, 0);
	atomic_set(&data->notify_dtr_state, 0);
	wake_lock_init(&data->wlock_dtr, WAKE_LOCK_SUSPEND, "quectel_dtr");
	wake_lock_init(&data->wlock_wakeup, WAKE_LOCK_SUSPEND, "quectel_wakeup");
	data->pdev = pdev;
	device_init_wakeup(&pdev->dev, 1);
	data->pdata = pdata;
	platform_set_drvdata(pdev, data);
	data->wakeup_flag = 0;
	data->dtr_flag = 0;

#ifdef MINIPCIE_MODE_PIN
#if 0/*Add duke default close*/
	if(quectel_wakeup_init(data) < 0)
	{
		printk("[Max][DTR] %s: init wakeup gpio fail\n", __func__);
		goto err_wakeup_init;
	}
#endif	
#endif

	if (quectel_sleep_state_init(data) < 0)
	{
		printk("[Max][DTR] %s: init sleep state gpio fail\n", __func__);
		goto err_sleep_ind_init;
	}

	if (quectel_dtr_init(data) < 0)
	{
		printk("[Max][DTR] %s: init dtr gpio fail\n", __func__);
		goto err_dtr_init;
	}

#ifdef MINIPCIE_MODE_PIN
	if (device_create_file(&pdev->dev, &dev_attr_wakeup_in) < 0)
	{
		printk("[Max][DTR] %s: dev file creation for wakeup_in failed\n", __func__);
	}
#endif

	if (device_create_file(&pdev->dev, &dev_attr_dtr_in) < 0)
	{
		printk("[Max][DTR] %s: dev file creation for dtr_in failed\n", __func__);
	}
	device_create_file(&pdev->dev, &dev_attr_dtr_state);
    
#ifdef MINIPCIE_MODE_PIN	
#if 1/*add duke20180629*/
     if (device_create_file(&pdev->dev, &dev_attr_wakeup_enable) < 0)
     {
            printk("[duke][WAKE] %s: dev file creation for wakeup_enable failed\n", __func__);
      }else{
            printk("[duke][WAKE] %s\n", "create  file wakeup_enable success");
      }
	  if (device_create_file(&pdev->dev, &dev_attr_wakeup_in_polarity) < 0)
     {
            printk("[duke][WAKE] %s: dev file creation for wakeup_in_polarity failed\n", __func__);
      }else{
            printk("[duke][WAKE] %s\n", "create  file wakeup_in_polarity success");
      }
#endif
     if (device_create_file(&pdev->dev, &dev_attr_sleep_polarity) < 0)//lory 20160921
     {
            printk("[lory][sleep] %s: dev file creation for sleep_polarity failed\n", __func__);
      }else{
            printk("[lory][sleep] %s\n", "create  file sleep_polarity success");
      }
#elif defined(QUECTEL_OPEN_SLEEP_IND_FEATURE)
     if (device_create_file(&pdev->dev, &dev_attr_sleep_polarity) < 0)//lory 20160921
     {
            printk("[lory][sleep] %s: dev file creation for sleep_polarity failed\n", __func__);
      }else{
            printk("[lory][sleep] %s\n", "create  file sleep_polarity success");
      }
#endif
	printk("[Max][DTR] %s: OK ###############################\n", __func__);
	return 0;

err_dtr_init:
err_sleep_ind_init:
#ifdef MINIPCIE_MODE_PIN
err_wakeup_init:
#endif
	platform_set_drvdata(pdev, NULL);
	return -1;
}

static int quectel_power_manager_remove(struct platform_device *pdev)
{
	struct quectel_power_manager *data = platform_get_drvdata(pdev);
#ifdef MINIPCIE_MODE_PIN
	struct quectel_power_manager_pdata *pdata = data->pdata;
#endif

#ifdef MINIPCIE_MODE_PIN	
	free_irq(data->irq_wakeup, data);
	gpio_free(pdata->gpio_wakeup);
	disable_irq_wake(data->irq_wakeup);
#endif
	platform_set_drvdata(pdev, NULL);

#ifdef MINIPCIE_MODE_PIN	
	device_remove_file(&pdev->dev, &dev_attr_wakeup_in);
#endif

	device_remove_file(&pdev->dev, &dev_attr_dtr_in);
#ifdef DTR_ESCAPE_MODE//francis
	dtr_smd_release();
#endif
	return 0;
}

static int quectel_power_manager_suspend(struct platform_device *pdev, pm_message_t state)
{
	struct quectel_power_manager *data = platform_get_drvdata(pdev);
	struct quectel_power_manager_pdata *pdata = data->pdata;

#if 1 //carl, DTR maybe change from quectel_dtr_show to here, so check again, maybe use wake_lock or level triger irq
	if (!gpio_get_value(pdata->gpio_dtr)) {
		dev_err(&pdev->dev, "Abort PM suspend!! (DTR is 0)\n");
		return -EBUSY;
	}
#endif

#ifdef MINIPCIE_MODE_PIN
	if(wakeup_enable_value==1)
	{
		/*Wythe.WANG-20190717:Only get wakeup in level when wakeup in is enable*/
		if (gpio_get_value(pdata->gpio_wakeup) == wakeup_in_active_level) {
			dev_err(&pdev->dev, "Abort PM suspend!! (WakeupIn is active:%d)\n", wakeup_in_active_level);
			return -EBUSY;
		}
		//enable_irq(data->irq_wakeup);
		if (enable_irq_wake(data->irq_wakeup) < 0)
		{
			printk("[Max][DTR] %s: enable irq wake fail\n", __func__);
			return -1;
		}
		else
		{
			data->wakeup_flag = 1;
		}
	}
#endif

	//enable_irq(data->irq_dtr);
	if (enable_irq_wake(data->irq_dtr) < 0)
	{
		printk("[Max][DTR] %s: enable irq dtr fail\n", __func__);
		return -1;
	}
	else
	{
		data->dtr_flag = 1;
	}

#ifdef MINIPCIE_MODE_PIN
        if (pdata->gpio_sleep_state)
        {
            if(sleep_pola_ind)
            {
                gpio_set_value(pdata->gpio_sleep_state, 0);
            }
            else
            {
                gpio_set_value(pdata->gpio_sleep_state, 1);
            }
        }
#elif defined(QUECTEL_OPEN_SLEEP_IND_FEATURE)
	     if (pdata->gpio_sleep_state)
        {
            if(sleep_pola_ind)
            {
                gpio_set_value(pdata->gpio_sleep_state, 0);
            }
            else
            {
                gpio_set_value(pdata->gpio_sleep_state, 1);
            }
        }			
 #else
       gpio_set_value(pdata->gpio_sleep_state, 1);
#endif

	/******************************************************************************************
	asa.wang-2018/04/02:when the system goes to sleep, turn off the net status indicator
	Refer to [Issue-Depot].[IS0000099][Submitter:asa.wang,Date:2018-04-02]
	******************************************************************************************/
	gpio_set_value(pdata->gpio_net_status, 0);

	return 0;
}

static int quectel_power_manager_resume(struct platform_device *pdev)
{
	struct quectel_power_manager *data = platform_get_drvdata(pdev);
	struct quectel_power_manager_pdata *pdata = data->pdata;
#ifdef MINIPCIE_MODE_PIN
       if (pdata->gpio_sleep_state)
        {
            if(sleep_pola_ind)
            {
                gpio_set_value(pdata->gpio_sleep_state, 1);
            }
            else
            {
                gpio_set_value(pdata->gpio_sleep_state, 0);
            }
        }
#elif defined(QUECTEL_OPEN_SLEEP_IND_FEATURE)
	  if (pdata->gpio_sleep_state)
        {
            if(sleep_pola_ind)
            {
                gpio_set_value(pdata->gpio_sleep_state, 1);
            }
            else
            {
                gpio_set_value(pdata->gpio_sleep_state, 0);
            }
        }  	   
#else
       gpio_set_value(pdata->gpio_sleep_state, 0);
#endif

#ifdef MINIPCIE_MODE_PIN
	if(wakeup_enable_value==1)
	{
		if (disable_irq_wake(data->irq_wakeup) < 0)
		{
			printk("[Max][DTR] %s: disable irq wake fail\n", __func__);
			return -1;
		}
	}
#endif

	if (disable_irq_wake(data->irq_dtr) < 0)
	{
		printk("[Max][DTR] %s: disable irq dtr fail\n", __func__);
		return -1;
	}

	return 0;
}

static const struct of_device_id quec_power_manager_match[] = 
{
	{.compatible = "quec,quectel-power-manager",},
	{},
};

static struct platform_driver quectel_power_manager_driver = {
	.probe		= quectel_power_manager_probe,
	.remove		= quectel_power_manager_remove,
	.suspend	= quectel_power_manager_suspend,
	.resume		= quectel_power_manager_resume,
	.driver		= {
		.name = MSM9615_QUECTEL_POWER_MANAGER_NAME,
		.owner = THIS_MODULE,
		.of_match_table = quec_power_manager_match,
	},
};

module_platform_driver(quectel_power_manager_driver);

MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("quectel sleep driver");
MODULE_VERSION("1.0");
