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

static struct fb_info *myfb; //定义一个 fb_info
static struct task_struct *fb_thread; //定义一个线程刷新屏幕
static struct spi_device *fbspi; //保存 spi 驱动指针
static struct gpio_desc *dc_pin; //dc 引脚
static struct gpio_desc *reset_pin; //reset 引脚
static u32 pseudo_palette[16]; //调色板缓存区

static void fb_write_reg(struct spi_device *spi, u8 reg)
{
    gpiod_set_value(dc_pin, 0); //低电平，命令
    spi_write(spi, &reg, 1);
}

static void fb_write_data(struct spi_device *spi, u16 data)
{ 
    u8 buf[2];
    buf[0] = ((u8)(data>>8));
    buf[1] = ((u8)(data&0x00ff));
    gpiod_set_value(dc_pin, 1); //高电平，数据
    spi_write(spi, &buf[0], 1);
    spi_write(spi, &buf[1], 1); 
}
static void fb_set_win(struct spi_device *spi, u8 xStar, u8 yStar,u8 xEnd,u8 yEnd)
{
    fb_write_reg(spi,0x2a); 
    fb_write_data(spi,xStar);
    fb_write_data(spi,xEnd);
    fb_write_reg(spi,0x2b); 
    fb_write_data(spi,yStar);
    fb_write_data(spi,yEnd);
    fb_write_reg(spi,0x2c); 
}

static void myfb_init(struct spi_device *spi)
{
    gpiod_set_value(reset_pin, 0); //设低电平
    msleep(100);
    gpiod_set_value(reset_pin, 1); //设高电平
    msleep(50);
    /* 写寄存器，初始化 */
    fb_write_reg(spi,0x36);
    fb_write_data(spi,0x0000);
    fb_write_reg(spi,0x3A);
    fb_write_data(spi,0x0500);
    fb_write_reg(spi,0xB2);
    fb_write_data(spi,0x0C0C);
    fb_write_data(spi,0x0033);
    fb_write_data(spi,0x3300);
    fb_write_data(spi,0x0033);
    fb_write_data(spi,0x3300);
    fb_write_reg(spi,0xB7);
    fb_write_data(spi,0x3500);
    fb_write_reg(spi,0xB8);
    fb_write_data(spi,0x1900);
    fb_write_reg(spi,0xC0);
    fb_write_data(spi,0x2C00);
    fb_write_reg(spi,0xC2);
    fb_write_data(spi,0xC100);
    fb_write_reg(spi,0xC3);
    fb_write_data(spi,0x1200);
    fb_write_reg(spi,0xC4);
    fb_write_data(spi,0x2000);
    fb_write_reg(spi,0xC6);
    fb_write_data(spi,0x0F00);
    fb_write_reg(spi,0xD0);
    fb_write_data(spi,0xA4A1);
    fb_write_reg(spi,0xE0);
    fb_write_data(spi,0xD004);
    fb_write_data(spi,0x0D11);
    fb_write_data(spi,0x132B);
    fb_write_data(spi,0x3F54);
    fb_write_data(spi,0x4C18);
    fb_write_data(spi,0x0D0B);
    fb_write_data(spi,0x1F23);
    fb_write_reg(spi,0xE1);
    fb_write_data(spi,0xD004);
    fb_write_data(spi,0x0C11);
    fb_write_data(spi,0x132C);
    fb_write_data(spi,0x3F44);
    fb_write_data(spi,0x512F);
    fb_write_data(spi,0x1F1F);
    fb_write_data(spi,0x2023);
    fb_write_reg(spi,0x21);
    fb_write_reg(spi,0x11);
    mdelay(50); 
    fb_write_reg(spi,0x29);
    mdelay(200);
}

void fb_refresh(struct fb_info *fbi, struct spi_device *spi)
{
    int x, y;
    u16 *p = (u16 *)(fbi->screen_base);
    fb_set_win(spi, 0,0,239,239);
    for (y = 0; y < fbi->var.yres; y++)
    {
        for (x = 0; x < fbi->var.xres; x++)
        {
            fb_write_data(spi,*p);
            p++;
        }
    }
}

int thread_func_fb(void *data)
{
    struct fb_info *fbi = (struct fb_info *)data;
    while (1)
    {
        if (kthread_should_stop())
        break;
        fb_refresh(fbi,fbspi);
    }
    return 0;
}

static inline unsigned int chan_to_field(unsigned int chan, struct fb_bitfield *bf)
{
    chan &= 0xffff;
    chan >>= 16 - bf->length;
    return chan << bf->offset;
}

static u32 pseudo_palette[16];
static int myfb_setcolreg(u32 regno, u32 red,u32 green, u32 blue,u32 transp, struct fb_info *info)
{
    unsigned int val;
    if (regno > 16)
    {
        return 1;
    }
    val = chan_to_field(red, &info->var.red);
    val |= chan_to_field(green, &info->var.green);
    val |= chan_to_field(blue, &info->var.blue);
    pseudo_palette[regno] = val;
    return 0;
}

static struct fb_ops myfb_ops = {
    .owner = THIS_MODULE,
    .fb_write = fb_sys_write,
    .fb_setcolreg = myfb_setcolreg, /*设置颜色寄存器*/
    .fb_fillrect = sys_fillrect, /*用像素行填充矩形框，通用库函数*/
    .fb_copyarea = sys_copyarea, /*将屏幕的一个矩形区域复制到另一个区域，通用库函数*/
    .fb_imageblit = sys_imageblit, /*显示一副图像，通用库函数*/
};

static void myfb_update(struct fb_info *fbi, struct list_head *pagelist)
{
    //比较粗暴的方式，直接全部刷新
    fbi->fbops->fb_pan_display(&fbi->var,fbi); //将应用层数据刷到 FrameBuffer 缓存中
}

static struct fb_deferred_io myfb_defio = {
    .delay = HZ/20,
    .deferred_io = &myfb_update,
};
static struct fb_var_screeninfo myfb_var = {
    .rotate = 0,
    .xres = 240,
    .yres = 240,
    .xres_virtual = 240,
    .yres_virtual = 240,
    .bits_per_pixel = 16,
    .nonstd = 1,
/* RGB565 */
    .red.offset = 11,
    .red.length = 5,
    .green.offset = 5,
    .green.length = 6,
    .blue.offset = 0,
    .blue.length = 5,
    .transp.offset = 0,
    .transp.length = 0,
    .activate = FB_ACTIVATE_NOW,
    .vmode = FB_VMODE_NONINTERLACED,
};

static struct fb_fix_screeninfo myfb_fix = {
    .type = FB_TYPE_PACKED_PIXELS,
    .visual = FB_VISUAL_TRUECOLOR,
    .line_length = 240*2,//16 bit = 2byte
    .accel = FB_ACCEL_NONE,//没有使用硬件加速
    .id = "myfb",
};

static int myfb_probe(struct spi_device *spi)
{
    int ret;
    void *gmem_addr;
    u32 gmem_size;
    fbspi = spi; //保存 spi 驱动指针
    printk(KERN_ERR"register myfb_spi_probe!\n");
    //申请 GPIO 用作 DC 引脚
    dc_pin = devm_gpiod_get(&spi->dev,"dc",GPIOF_OUT_INIT_LOW);
    if(IS_ERR(dc_pin))
    {
        printk(KERN_ERR"fail to request dc-gpios!\n");
        return -1;
    }
    //申请 GPIO 用作 RESET 引脚
    reset_pin = devm_gpiod_get(&spi->dev,"reset",GPIOF_OUT_INIT_HIGH);
    if(IS_ERR(reset_pin))
    {
        printk(KERN_ERR"fail to request reset-gpios!\n");
        return -1;
    }
    gpiod_direction_output(dc_pin,0);//设置输出方向
    gpiod_direction_output(reset_pin,1);//设置输出方向
    printk(KERN_INFO"register myfb_probe dev!\n");
    myfb = framebuffer_alloc(sizeof(struct fb_info), &spi->dev);//向内核申请 fb_info 结构体
    //初始化底层操作结构体
    myfb->fbops = &myfb_ops; //指定底层操作结构体
    gmem_size = 240 * 240 * 2; //设置显存大小，16bit 占 2 字节
    gmem_addr = kmalloc(gmem_size,GFP_KERNEL); //分配 Frame Buffer 显存
    if(!gmem_addr)
    {
        printk(KERN_ERR"fail to alloc fb buffer!\n");
    }
    myfb->pseudo_palette = pseudo_palette;
    myfb->var = myfb_var; //设置分辨率参数
    myfb->fix = myfb_fix; //设置显示参数
    myfb->screen_buffer = gmem_addr;//设置显存地址
    myfb->screen_size = gmem_size;//设置显存大小
    myfb->fix.smem_len = gmem_size;//设置应用层显存大小
    myfb->fix.smem_start = (u32)gmem_addr;//设置应用层数据地址
    memset((void *)myfb->fix.smem_start, 0, myfb->fix.smem_len); //清楚数据缓存
    myfb_init(spi); //初始化显示屏
    myfb->fbdefio = &myfb_defio; //设置刷新参数
    fb_deferred_io_init(myfb); //初始化刷新机制
    ret = register_framebuffer(myfb); //注册 fb 驱动
    if(ret)
    {
        framebuffer_release(myfb);
        unregister_framebuffer(myfb);
        devm_gpiod_put(&spi->dev, dc_pin);
        devm_gpiod_put(&spi->dev, reset_pin);
        printk(KERN_ERR"fail to register fb dev!\n");
        return -1;
    }
    //开启一个线程用来刷新显示屏
    fb_thread= kthread_run(thread_func_fb, myfb, spi->modalias);
    return 0;
}

static void myfb_remove(struct spi_device *spi)
{
    fb_deferred_io_cleanup(myfb); //清除刷新机制
    unregister_framebuffer(myfb);
    devm_gpiod_put(&spi->dev, dc_pin);
    devm_gpiod_put(&spi->dev, reset_pin);
}

struct of_device_id myfb_match[] = {
    {.compatible = "screen,ST7789V3"}, 
    {},
};

struct spi_driver myfb_drv = {
    .probe = myfb_probe,
    .remove = myfb_remove,
    .driver = {
        .owner = THIS_MODULE,
        .name = "st7789v",
        .of_match_table = myfb_match,
    },
};

module_spi_driver(myfb_drv);
MODULE_LICENSE("GPL"); //不加的话加载会有错误提醒
MODULE_AUTHOR("1477153217@qq.com"); //作者
MODULE_VERSION("0.1"); //版本
MODULE_DESCRIPTION("myfb_spi_driver"); //简单的描述

