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
#include <linux/delay.h>

#define ST7789_CNT      1
#define ST7789_NAME     "st7789v"

struct st7789v_dev {
    int major;
    int minor;
    dev_t devid;
    struct cdev cdev;
    struct class *class;
    struct device *device;
    void *private_data;
    int dc_gpio;
    int reset_gpio;
    int led_gpio;
    struct device_node *nd; 
};
static struct st7789v_dev st7789vdev;


struct st7789v_frame {
    uint16_t xsta;
    uint16_t ysta;
    uint16_t xend;
    uint16_t yend;
    uint8_t color[240*240*2];
};
static struct st7789v_frame s_frame;

typedef struct {
    uint8_t cmd;
    uint8_t data[16];
    uint8_t databytes; //No of data in data; bit 7 = delay after set; 0xFF = end of cmds.
} lcd_init_cmd_t;

//Place data into DRAM. Constant data gets placed into DROM by default, which is not accessible by DMA.
static const lcd_init_cmd_t st_init_cmds[]={
    /* Sleep Out */
    {0x11, {0}, 0x80},
    /* Memory Data Access Control, MX=MV=1, MY=ML=MH=0, RGB=0 */
    {0x36, {0x00, 0x00}, 2},
    /* Interface Pixel Format, 16bits/pixel for RGB/MCU interface */
    {0x3A, {0x05, 0x00}, 2},
    /* Porch Setting */
    {0xB2, {0x0c, 0x0c, 0x00, 0x33, 0x33, 0x00, 0x00, 0x33, 0x33, 0x00}, 10},
    /* Gate Control, Vgh=13.65V, Vgl=-10.43V */
    {0xB7, {0x35, 0x00}, 2},
    /* VCOM Setting, VCOM=1.175V */
    {0xBB, {0x19, 0x00}, 2},
    /* LCM Control, XOR: BGR, MX, MH */
    {0xC0, {0x2C}, 1},
    /* VDV and VRH Command Enable, enable=1 */
    {0xC2, {0x01}, 1},
    /* VRH Set, Vap=4.4+... */
    {0xC3, {0x12}, 1},
    /* VDV Set, VDV=0 */
    {0xC4, {0x20}, 1},
    /* Frame Rate Control, 60Hz, inversion=0 */
    {0xC6, {0x0F}, 1},
    /* Power Control 1, AVDD=6.8V, AVCL=-4.8V, VDDS=2.3V */
    {0xD0, {0xA4, 0xA1}, 2},
    /* Positive Voltage Gamma Control */
    {0xE0, {0xD0, 0x04, 0x0D, 0x11, 0x13, 0x2B, 0x3F, 0x54, 0x4C, 0x18, 0x0D, 0x0B, 0x1F, 0x23}, 14},
    /* Negative Voltage Gamma Control */
    {0xE1, {0xD0, 0x04, 0x0C, 0x11, 0x13, 0x2C, 0x3F, 0x44, 0x51, 0x2F, 0x1F, 0x1F, 0x20, 0x23}, 14},
    /* Display On */
    {0x21, {0}, 0},
    {0x11, {0}, 0},
    {0x29, {0}, 0x80},
    {0x2a, {0, 0, 0, 240}, 4},
    {0x2b, {0, 0, 0, 240}, 4},
    {0x2c, {0}, 0},
    {0, {0}, 0xff}
};

static int st7789v_write_regs(struct st7789v_dev *dev, void *buf, uint16_t len);
static int st7789v_write_one_regs(struct st7789v_dev *dev, void *buf);
static void st7789_cmd(struct st7789v_dev *dev, const uint8_t cmd);
static void st7789_data(struct st7789v_dev *dev, const uint8_t *data, uint16_t len);

static int st7789v_open(struct inode *inode, struct file *file){
    int ret = 0;
    file->private_data = &st7789vdev;
    printk("st7789v_open!\n");
    return ret;
}

ssize_t st7789v_write(struct file *file,const char __user *buf,size_t size,loff_t *off){

    struct st7789v_dev *dev = file->private_data;
    if(copy_from_user(&s_frame, buf, size)!=0){
            printk("copy_to_user error!\n");
            return -1;
    }
    ssize_t ret = 0;
    int cmd = 0;
    const lcd_init_cmd_t* lcd_init_cmds;
    lcd_init_cmd_t lcd_address[]={
	{0x2a, {(s_frame.xsta >> 8) & 0xff, s_frame.xsta & 0xff, (s_frame.xend >> 8) & 0xff, s_frame.xend & 0xff}, 4},
	{0x2b, {(s_frame.ysta >> 8) & 0xff, s_frame.ysta & 0xff, (s_frame.yend >> 8) & 0xff, s_frame.yend & 0xff}, 4},
	{0x2c, {0}, 0},
	{0, {0}, 0xff}
	};
	lcd_init_cmds = lcd_address;
    while (lcd_init_cmds[cmd].databytes!=0xff) {
        st7789_cmd(dev, lcd_init_cmds[cmd].cmd);
        st7789_data(dev, lcd_init_cmds[cmd].data, lcd_init_cmds[cmd].databytes&0x1F);
        cmd++;
    }
    
    gpio_set_value(dev->dc_gpio, 1);//写数据
    uint32_t n_set = 0,leng = (s_frame.xend+1-s_frame.xsta)*(s_frame.yend+1-s_frame.ysta) * 2;
    while(n_set <= leng - 64){
        st7789v_write_regs(dev,(uint8_t*)&s_frame.color[n_set],64);
        n_set += 64;
    }
    st7789v_write_regs(dev,(uint8_t*)&s_frame.color[n_set],leng - n_set);

    printk("size:%d\r\n", (unsigned int)((s_frame.xend+1-s_frame.xsta)*(s_frame.yend+1-s_frame.ysta)));
    if(ret < 0){
        printk("st7789v_write_regs error!\n");
    }
    gpio_set_value(dev->led_gpio, 1);
    printk("st7789v_write!\n");
    return ret;
}

static int st7789v_release(struct inode *, struct file *){
    int ret = 0;
    printk("st7789v release!\r\n");
    gpio_set_value(st7789vdev.led_gpio, 0);
    return ret;
}

static const struct file_operations st7789v_fops = {
    .owner      = THIS_MODULE,
    .open       = st7789v_open,
    .write      = st7789v_write,
    .release    = st7789v_release,
};


static int st7789v_write_regs(struct st7789v_dev *dev, void *buf, uint16_t len)
{
    int ret = 0;
    struct spi_message m;
    struct spi_transfer *t;
    struct spi_device *spi = (struct spi_device *)dev->private_data;

    t = kzalloc(sizeof(struct spi_transfer), GFP_KERNEL);

    t->tx_buf = buf;
    t->len = len;
    spi_message_init(&m);
    spi_message_add_tail(t, &m);
    spi_sync(spi, &m);

    kfree(t);

    return ret;
}

static int st7789v_write_one_regs(struct st7789v_dev *dev, void *buf)
{
    int ret = 0;
    struct spi_message m;
    struct spi_transfer *t;
    struct spi_device *spi = (struct spi_device *)dev->private_data;

    t = kzalloc(sizeof(struct spi_transfer), GFP_KERNEL);

    t->tx_buf = buf;
    t->len = 1;
    spi_message_init(&m);
    spi_message_add_tail(t, &m);
    spi_sync(spi, &m);

    kfree(t);

    return ret;
}

static void st7789_cmd(struct st7789v_dev *dev, const uint8_t cmd)
{
	gpio_set_value(dev->dc_gpio, 0);//写命令
	st7789v_write_regs(dev, (void *)&cmd, 1);
}
	
static void st7789_data(struct st7789v_dev *dev, const uint8_t *data, uint16_t len)
{
	gpio_set_value(dev->dc_gpio, 1);//写数据
	st7789v_write_regs(dev, (void *)data, len);
}

void st7789v_device_init(struct st7789v_dev *dev)
{
    printk("st7789v device init\r\n");
    uint16_t cmd = 0;
    uint32_t n_set = 0,leng = 240 * 240 * 2;
    const lcd_init_cmd_t* lcd_init_cmds;
	
    //Reset the display
    gpio_set_value(dev->reset_gpio, 0);//复位
	mdelay(1);
	gpio_set_value(dev->reset_gpio, 1);
	mdelay(1);

   //Point to st_init_cmds
    lcd_init_cmds = st_init_cmds;

    //Send all the commands
    while (lcd_init_cmds[cmd].databytes!=0xff) {
        st7789_cmd(dev, lcd_init_cmds[cmd].cmd);
        st7789_data(dev, lcd_init_cmds[cmd].data, lcd_init_cmds[cmd].databytes&0x1F);
        if (lcd_init_cmds[cmd].databytes&0x80) {
            mdelay(100);
        }
        cmd++;
    }

    gpio_set_value(dev->dc_gpio, 1);//写数据
    while(n_set <= leng - 64){
        st7789v_write_regs(dev,(uint8_t*)&s_frame.color[n_set],64);
        n_set += 64;
    }
    st7789v_write_regs(dev,(uint8_t*)&s_frame.color[n_set],leng - n_set);
}

static int st7789v_spi_prove(struct spi_device *spi)
{
    int ret = 0;
    printk("spi prove\r\n");
    st7789vdev.major = 0;
    if(st7789vdev.major){
        st7789vdev.devid = MKDEV(st7789vdev.major, 0);
        ret = register_chrdev_region(st7789vdev.devid, ST7789_CNT, ST7789_NAME);
    } else{
        ret = alloc_chrdev_region(&st7789vdev.devid, 0, ST7789_CNT, ST7789_NAME);
        st7789vdev.major = MAJOR(st7789vdev.devid);
        st7789vdev.minor = MINOR(st7789vdev.devid);
    }
    if(ret < 0){
        printk("st7789v chrdev_region err!\r\n");
        goto fail_devid;
    }
    printk("st7789vdev major=%d, minor=%d", st7789vdev.major, st7789vdev.minor);


    st7789vdev.cdev.owner = THIS_MODULE;
    cdev_init(&st7789vdev.cdev, &st7789v_fops);
    ret = cdev_add(&st7789vdev.cdev, st7789vdev.devid, ST7789_CNT);
    if(ret < 0){
        goto fail_cdev;
    }


    st7789vdev.class = class_create(THIS_MODULE, ST7789_NAME);
    if(IS_ERR(st7789vdev.class)){
        ret = PTR_ERR(st7789vdev.class);
        goto fail_class;
    }


    st7789vdev.device = device_create(st7789vdev.class, NULL,
                st7789vdev.devid, NULL, ST7789_NAME);
    if(IS_ERR(st7789vdev.device)){
        ret = PTR_ERR(st7789vdev.device);
        goto fail_device;
    }

    // st7789vdev.nd = of_get_parent(spi->dev.of_node);
    st7789vdev.nd = spi->dev.of_node;
    st7789vdev.dc_gpio = of_get_named_gpio(st7789vdev.nd, "dc-gpios", 0);
    if(st7789vdev.dc_gpio < 0){
        printk("cant't get dc-gpios\r\n");
        goto fail_gpio;
    }
    ret = gpio_request(st7789vdev.dc_gpio, "dc");
    if(ret < 0){
        printk("dc gpio request fail\r\n");
    }

    st7789vdev.reset_gpio = of_get_named_gpio(st7789vdev.nd, "reset-gpios", 0);
    if(st7789vdev.reset_gpio < 0){
        printk("cant't get reset-gpios\r\n");
        goto fail_gpio;
    }
    ret = gpio_request(st7789vdev.reset_gpio, "reset");
    if(ret < 0){
        printk("reset gpio request fail\r\n");
    }

    st7789vdev.led_gpio = of_get_named_gpio(st7789vdev.nd, "led-gpios", 0);
    if(st7789vdev.led_gpio < 0){
        printk("cant't get led-gpios\r\n");
        goto fail_gpio;
    }
    ret = gpio_request(st7789vdev.led_gpio, "led");
    if(ret < 0){
        printk("led gpio request fail\r\n");
    }

    ret = gpio_direction_output(st7789vdev.dc_gpio, 1);
    ret = gpio_direction_output(st7789vdev.reset_gpio, 1);
    ret = gpio_direction_output(st7789vdev.led_gpio, 0);

    spi->mode = SPI_MODE_3;
    spi->bits_per_word = 8;
    spi->max_speed_hz = 100000000;
    spi_setup(spi);
    st7789vdev.private_data = spi;

    st7789v_device_init(&st7789vdev);

    return 0;

fail_gpio:
fail_device:
    class_destroy(st7789vdev.class);
fail_class:
    cdev_del(&st7789vdev.cdev);
fail_cdev:
    unregister_chrdev_region(st7789vdev.devid, ST7789_CNT);
fail_devid:
    return ret;
}

static void st7789v_spi_remove(struct spi_device *spi)
{
    int ret = 0;
    gpio_set_value(st7789vdev.led_gpio, 0);//写命令
    gpio_set_value(st7789vdev.reset_gpio, 0);//复位
	mdelay(1);
	gpio_set_value(st7789vdev.reset_gpio, 1);
	mdelay(1);

    gpio_free(st7789vdev.dc_gpio);
    gpio_free(st7789vdev.reset_gpio);
    gpio_free(st7789vdev.led_gpio);

    cdev_del(&st7789vdev.cdev);

    unregister_chrdev_region(st7789vdev.devid, ST7789_CNT);

    device_destroy(st7789vdev.class, st7789vdev.devid);

    class_destroy(st7789vdev.class);
    printk("%d:spi remove\r\n", ret);
}

static const struct spi_device_id st7789v_id[] = {
    {"screen,ST7789V3", 0},
    {}
};


static const struct of_device_id st7789v_of_match[] = {
    {.compatible = "screen,ST7789V3"},
    { }
};



struct spi_driver st7789v_driver = {
	.id_table       = st7789v_id,
	.probe          = st7789v_spi_prove,
	.remove         = st7789v_spi_remove,
	.driver = {
        .name       = "st7789v",
        .owner      = THIS_MODULE,
        .of_match_table = st7789v_of_match,
    },
    
};


static int __init st7789v_init(void)
{
    int ret = 0;
    spi_register_driver(&st7789v_driver);
    return ret;
}


static void __exit st7789v_exit(void)
{
    // int ret = 0;
    spi_unregister_driver(&st7789v_driver);
}

module_init(st7789v_init);
module_exit(st7789v_exit);
MODULE_LICENSE("GPL"); //不加的话加载会有错误提醒
MODULE_AUTHOR("2727802848@qq.com"); //作者
MODULE_VERSION("0.1"); //版本


