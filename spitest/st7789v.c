#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <asm/io.h> //含有 ioremap 函数 iounmap 函数
#include <asm/uaccess.h> //含有 copy_from_user 函数和含有 copy_to_user 函数
#include <linux/device.h> //含有类相关的设备函数
#include <linux/cdev.h>
#include <linux/platform_device.h> //包含 platform 函数
#include <linux/of.h> //包含设备树相关函数
#include <linux/fb.h> //包含 frame buffer
#include <linux/spi/spi.h>
#include <linux/regmap.h>
#include <linux/gpio.h>
#include <linux/of_gpio.h>


static int st7789v_spi_prove(struct spi_device *spi)
{
    int ret = 0;
    printk("spi prove\r\n");
    return ret;
}

static void st7789v_spi_remove(struct spi_device *spi)
{
    int ret = 0;
    printk("spi remove\r\n");
}

static const struct spi_device_id st7789v_id[] = {
    {"sitronix,st7789v", 0},
    {}
};


static const struct of_device_id st7789v_of_match[] = {
    {.compatible = "sitronix,st7789v"},
    { }
};



struct spi_driver st7789v_driver = {
	.probe          = st7789v_spi_prove,
	.remove         = st7789v_spi_remove,
	.driver = {
        .name       = "st7789v",
        .owner      = THIS_MODULE,
        .of_match_table = st7789v_of_match,
    },
    .id_table       = st7789v_id,
};


static int __init st7789v_init(void)
{
    int ret = 0;
    spi_register_driver(&st7789v_driver);
    return ret;
}


static void __exit st7789v_exit(void)
{
    int ret = 0;
    spi_unregister_driver(&st7789v_driver);
}

module_init(st7789v_init);
module_exit(st7789v_exit);
MODULE_LICENSE("GPL"); //不加的话加载会有错误提醒
MODULE_AUTHOR("2727802848@qq.com"); //作者
MODULE_VERSION("0.1"); //版本


