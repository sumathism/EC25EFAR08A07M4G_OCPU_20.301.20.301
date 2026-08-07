/***************************************************
add by hans
 ***************************************************/
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/miscdevice.h>
#include <linux/workqueue.h>
#include <linux/uaccess.h>
#include <soc/qcom/smd.h>
#include <linux/string.h>


#include <linux/platform_device.h>
#include <linux/err.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <asm/atomic.h>
#include <linux/init.h>
#include <linux/spinlock.h>

#include <linux/gpio.h>
#include <linux/of.h>
#include <linux/of_gpio.h>

#include <asm/irq.h>
#include <linux/kernel.h>
#include <linux/kthread.h> 
#include <linux/irq.h>
#include <linux/io.h>
#include <linux/moduleparam.h>
#include <linux/types.h>

#include <linux/clk.h>
#include <linux/irqdomain.h>
#include <linux/irqchip/chained_irq.h>
#include <linux/gpio/driver.h>
#include <linux/of_device.h>
#include <linux/bug.h>
#include <linux/module.h>

#include <linux/quectel/quec_raw_data_item.h>
#include <linux/quectel/quec_raw_flash_data.h>


#define FAST_SHUTDOWN_MAX_GPIO_NUM			79
#define FAST_SHUTDOWN_MIN_GPIO_NUM			0
#define MDM9X07_GPIO_REG_BASE_ADDRESS			0x1000000
#define MDM9X07_GPIO_REG_ADDRESS_WIDTH	    		0x1000

#define FAST_SHUTDOWN_GPIO_NAME				"GPIO_FAST_SHUTDOWN"
#define FAST_SHUTDOWN_GPIO_NAME_HANDLER		"GPIO_FAST_SHUTDOWN_HANDLER"


extern void ql_set_fastshutdown_with_pin(void);

static volatile uint32_t *GPIO_CFGn_vreg;
static volatile uint32_t *GPIO_IN_OUTn_vreg;

/*
 * GPIO DIRECTION registers
 */
#define GPIO_DIRECTION_INPUT              0x0
#define GPIO_DIRECTION_OUTPUT             0x3

/*
 * GPIO PULL registers
 */
#define GPIO_NO_PULL                      0x0
#define GPIO_PULL_DOWN                    0x1
#define GPIO_KEEPER                       0x2
#define GPIO_PULL_UP                      0x3

/*
 * GPIO strength registers
 */
#define GPIO_DRV_2_MA                     0x0
#define GPIO_DRV_4_MA                     0x40
#define GPIO_DRV_6_MA                     0x80
#define GPIO_DRV_8_MA                     0xc0
#define GPIO_DRV_10_MA                    0x100
#define GPIO_DRV_12_MA                    0x140
#define GPIO_DRV_14_MA                    0x180
#define GPIO_DRV_16_MA                    0x1c0

enum mdm_gpio_drv_strength_type {
    DRV_2_MA = 0,	/* drive strength to 2mA */
    DRV_4_MA,           /* drive strength to 4mA */
    DRV_6_MA,           /* drive strength to 6mA */
    DRV_8_MA,           /* drive strength to 8mA */
    DRV_10_MA,          /* drive strength to 10mA */
    DRV_12_MA,          /* drive strength to 12mA */
    DRV_14_MA,          /* drive strength to 14mA */
    DRV_16_MA,          /* drive strength to 16mA */
};

enum mdm_gpio_pull_type {
    NO_PULL = 0,        /* The pad configured NO_PULL */
    PULL_DOWN,          /* The pad configured PULL_DOWN */
    KEEPER,             /* The pad configured KEEPER */
    PULL_UP,            /* The pad configured PULL_UP */
};

enum mdm_gpio_in_out_type {
    GPIO_IN = 0,        /* Allows you to read the Input value of the GPIO */
    GPIO_OUT,           /* Controls the value of the GPIO Output */
};

/**
* struct mdm_gpio_hwdata: gpio information
* @addr: pipe description.
* @drv_strength: Controls the GPIO pad drive strength. 
* @gpio_pull: The pad can be configured to employ an internal weak pull up, pulldown, or keeper function.
* @gpio_in_out: Controls the Out and In on the GPIO
*/
struct mdm_gpio_hwdata {
    uint32_t addr;
    enum mdm_gpio_drv_strength_type drv_strength;
    enum mdm_gpio_pull_type gpio_pull;
    enum mdm_gpio_in_out_type gpio_in_out;
	uint32_t gpio_num;
};

static struct mdm_gpio_hwdata *gpio_hwdata = NULL;

static irqreturn_t qfastshutdown_thread_func(int irq, void *data)
{
	if(gpio_get_value(gpio_hwdata->gpio_num) == 0)
	{
		mdelay(10);
		if(gpio_get_value(gpio_hwdata->gpio_num) == 0)
		{
			ql_set_fastshutdown_with_pin();
		}
	}

	return IRQ_HANDLED;
}

static irqreturn_t qfastshutdown_handler(int irq, void *dev_id)
{
	// return IRQ_RETVAL(IRQ_HANDLED);
	return IRQ_WAKE_THREAD;
}

static struct mdm_gpio_hwdata *mdm_gpio_dt_to_hwdata(struct platform_device *pdev)
{
    int ret = 0;
    int rc = 0;
    uint32_t addr;
    struct device_node *node = pdev->dev.of_node;

    gpio_hwdata = devm_kzalloc(&pdev->dev, sizeof(*gpio_hwdata), GFP_KERNEL);
    if (!gpio_hwdata) {
        return NULL;
    }
    /* read addr */
	/* calc from gpio num.
    rc = of_property_read_u32(node, "qcom,gpio-addr",
                &gpio_hwdata->addr);
    if (rc) {
        dev_err(&pdev->dev, "Unable to retrieve register base.\n");
        return NULL;
    }
    */

    /* read drv_strength */
    rc = of_property_read_u32(node, "qcom,drv_strength", &gpio_hwdata->drv_strength);
    if (rc)
        gpio_hwdata->drv_strength = DRV_2_MA;

    /* read gpio_pull */
    rc = of_property_read_u32(node, "qcom,gpio_pull", &gpio_hwdata->gpio_pull);
    if (rc)
        gpio_hwdata->gpio_pull = PULL_DOWN;

    /* read gpio_in_out */
    rc = of_property_read_u32(node, "qcom,gpio_in_out", &gpio_hwdata->gpio_in_out);
    if (rc)
        gpio_hwdata->gpio_in_out = GPIO_IN;

	/* read gpio_num */
    rc = of_property_read_u32(node, "qcom,gpio_num", &gpio_hwdata->gpio_num);
    if (rc)
        return NULL;

	if(gpio_hwdata->gpio_num >= FAST_SHUTDOWN_MIN_GPIO_NUM && gpio_hwdata->gpio_num <= FAST_SHUTDOWN_MAX_GPIO_NUM)
		gpio_hwdata->addr = MDM9X07_GPIO_REG_BASE_ADDRESS + MDM9X07_GPIO_REG_ADDRESS_WIDTH*gpio_hwdata->gpio_num;


    return gpio_hwdata;
}

static void mdm_gpio_set_cfg(struct mdm_gpio_hwdata *gpio_hwdata)
{
    uint32_t reg = 0;
    switch (gpio_hwdata->drv_strength) {
        case DRV_2_MA:
            reg = reg | GPIO_DRV_2_MA;
            break;
        case DRV_4_MA:
            reg = reg | GPIO_DRV_4_MA;
            break;
        case DRV_6_MA:
            reg = reg | GPIO_DRV_6_MA;
            break;
        case DRV_8_MA:
            reg = reg | GPIO_DRV_8_MA;
            break;
        case DRV_10_MA:
            reg = reg | GPIO_DRV_10_MA;
            break;
        case DRV_12_MA:
            reg = reg | GPIO_DRV_12_MA;
            break;
        case DRV_14_MA:
            reg = reg | GPIO_DRV_14_MA;
            break;
        case DRV_16_MA:
            reg = reg | GPIO_DRV_16_MA;
            break;
        default:
            break;
    }

    switch (gpio_hwdata->gpio_pull) {
        case NO_PULL:
            reg = reg | GPIO_NO_PULL;
            break;
        case PULL_DOWN:
            reg = reg | GPIO_PULL_DOWN;
            break;
        case KEEPER:
            reg = reg | GPIO_KEEPER;
            break;
        case PULL_UP:
            reg = reg | GPIO_PULL_UP;
            break;
        default:
            break;
    }
    writel(reg, GPIO_CFGn_vreg);
}

static void mdm_gpio_set_direction(struct mdm_gpio_hwdata *gpio_hwdata)
{
    uint32_t reg = 0;
    if (gpio_hwdata->gpio_in_out) {
        reg = GPIO_DIRECTION_OUTPUT;
    } else {
        reg = GPIO_DIRECTION_INPUT;
    }
    writel(reg, GPIO_IN_OUTn_vreg);
}

static int mdm_fastshutdown_probe(struct platform_device *pdev)
{
    uint32_t val = 0;
    int ret = 0;
	unsigned gpio_irq;
    struct mdm_gpio_hwdata *gpio_hwdata;

    /* gpio dt to hwdata */
    gpio_hwdata = mdm_gpio_dt_to_hwdata(pdev);
    if (!gpio_hwdata){
        dev_err(&pdev->dev, "Failed dt to hwdata\n");
        ret = -EINVAL;
        goto err_exit;
    }

    /* GPIO_CFGn_vreg ioremap */
    GPIO_CFGn_vreg = (uint32_t *)devm_ioremap(&pdev->dev, gpio_hwdata->addr, 4);
    if(!GPIO_CFGn_vreg) {
        dev_err(&pdev->dev, "Failed to map GPIO_CFGn_vreg registers\n");
        ret = -ENOMEM;
        goto err_exit;
    }

    /* GPIO_IN_OUTn_vreg ioremap */
    GPIO_IN_OUTn_vreg = (uint32_t *)devm_ioremap(&pdev->dev, gpio_hwdata->addr + 4, 4);
    if(!GPIO_IN_OUTn_vreg) {
        dev_err(&pdev->dev, "Failed to map GPIO_IN_OUTn_vreg registers\n");
        ret = -ENOMEM;
        goto err_exit;
    }

    mdm_gpio_set_cfg(gpio_hwdata);
    mdm_gpio_set_direction(gpio_hwdata);

    dev_info(&pdev->dev, "Registered msm_gpio.\n");
	
	ret = gpio_request(gpio_hwdata->gpio_num, FAST_SHUTDOWN_GPIO_NAME);
	if (ret){
		printk("@quectel fastshutdown gpio_request failed \n");
		return -1;
	}

	gpio_irq = gpio_to_irq(gpio_hwdata->gpio_num);
	ret = request_threaded_irq(gpio_irq, qfastshutdown_handler, qfastshutdown_thread_func, IRQF_TRIGGER_FALLING, FAST_SHUTDOWN_GPIO_NAME_HANDLER, NULL);
	if (ret) {
		printk("@quectel fastshutdown gpio_to_irq failed \n");
		return -1;
	}
	printk("@quectel fastshutdown init success, gpio_num = %d, value = %d\n", gpio_hwdata->gpio_num, gpio_get_value(gpio_hwdata->gpio_num));

    return ret;
err_exit:
    return ret;
}

static const struct of_device_id mdm_fastshutdown_dt_ids[] = {
	{	.compatible = "quec,quectel-fastshutdown",
	},
	{}
};

static struct platform_driver mdm_fastshutdown_driver = {
        .probe          = mdm_fastshutdown_probe,
        .driver         = {
                .name   = "quectel-fastshutdown",
                .owner = THIS_MODULE,
                .of_match_table = mdm_fastshutdown_dt_ids,
        },
};

static int __init mdm_fastshutdown_init(void)
{
        return platform_driver_register(&mdm_fastshutdown_driver);
}
module_init(mdm_fastshutdown_init);

static void __exit mdm_fastshutdown_cleanup(void)
{
	platform_driver_unregister(&mdm_fastshutdown_driver);
}
module_exit(mdm_fastshutdown_cleanup);

MODULE_DESCRIPTION("QUECTEL fastshutdown Driver");
MODULE_LICENSE("GPL v2");

