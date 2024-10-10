#include <common.h>
#include <command.h>
#include <cpu_func.h>
#include <dm.h>
#include <errno.h>
#include <spi.h>
#include <video.h>
#include <asm/gpio.h>
#include <dm/device_compat.h>
#include <linux/delay.h>

struct spi_video_priv {
    struct udevice *dev;
    struct gpio_desc dc_gpio;
    struct gpio_desc reset_gpio;
};

typedef struct {
    uint8_t cmd;
    uint8_t data[16];
    uint8_t databytes; //No of data in data; bit 7 = delay after set; 0xFF = end of cmds.
} lcd_init_cmd_t;

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

static int spi_vedio_write_cmd(struct udevice *dev, u8 reg)
{
	struct spi_video_priv *priv = dev_get_priv(dev);
	int ret;

	ret = dm_gpio_set_value(&priv->dc_gpio, 0);
	if (ret) {
		dev_dbg(dev, "Failed to handle dc\n");
		return ret;
	}

	ret = dm_spi_xfer(dev, 8, &reg, NULL, SPI_XFER_BEGIN | SPI_XFER_END);
	if (ret)
		dev_dbg(dev, "Failed to write command\n");

	return ret;
}

static int spi_vedio_write_data(struct udevice *dev, u8 *data, u32 len)
{
	struct spi_video_priv *priv = dev_get_priv(dev);
	int ret;

	ret = dm_gpio_set_value(&priv->dc_gpio, 1);
	if (ret) {
		dev_dbg(dev, "Failed to handle dc\n");
		return ret;
	}

	ret = dm_spi_xfer(dev, 8 * len, data, NULL, SPI_XFER_BEGIN | SPI_XFER_END);
	if (ret)
		dev_dbg(dev, "Failed to write data\n");

	return ret;
}

static int spi_vedio_display_init(struct udevice *dev)
{
	struct spi_video_priv *priv = dev_get_priv(dev);
	lcd_init_cmd_t *lcd_init_cmds;
	uint16_t cmd = 0;

	//Point to st_init_cmds
    lcd_init_cmds = (lcd_init_cmd_t*)st_init_cmds;

	dm_gpio_set_value(&priv->reset_gpio, 0);
	mdelay(1000);
	dm_gpio_set_value(&priv->reset_gpio, 1);
	mdelay(1000);

    //Send all the commands
    while (lcd_init_cmds[cmd].databytes!=0xff) {
        spi_vedio_write_cmd(dev, lcd_init_cmds[cmd].cmd);
        spi_vedio_write_data(dev, lcd_init_cmds[cmd].data, lcd_init_cmds[cmd].databytes&0x1F);
        if (lcd_init_cmds[cmd].databytes&0x80) {
            mdelay(100);
        }
        cmd++;
    }
	printf("spi lcd vedio init complete!\n");
	return 0;
}

static int spi_vedio_startup(struct udevice *dev)
{
	struct spi_video_priv *priv = dev_get_priv(dev);
	int ret;

	ret = dm_gpio_set_value(&priv->reset_gpio, 1);
	if (ret)
		return ret;

	ret = dm_gpio_set_value(&priv->reset_gpio, 0);
	if (ret)
		return ret;

	ret = dm_spi_claim_bus(dev);
	if (ret) {
		dev_err(dev, "Failed to claim SPI bus: %d\n", ret);
		return ret;
	}

	ret = spi_vedio_display_init(dev);
	if (ret)
		return ret;

	dm_spi_release_bus(dev);

	return 0;
}

static int spi_video_sync(struct udevice *vid)
{
	struct video_priv *uc_priv = dev_get_uclass_priv(vid);
	struct spi_video_priv *priv = dev_get_priv(vid);
	struct udevice *dev = priv->dev;
	int ret;
	u8 *start = uc_priv->fb;

	ret = dm_spi_claim_bus(dev);
	if (ret) {
		dev_err(dev, "Failed to claim SPI bus: %d\n", ret);
		return ret;
	}
	spi_vedio_write_data(dev, start, (uc_priv->xsize * uc_priv->ysize) * 2);
	dm_spi_release_bus(dev);

	return 0;
}

static int spi_video_probe(struct udevice *dev)
{
    struct video_priv *uc_priv = dev_get_uclass_priv(dev);
    struct spi_video_priv *priv = dev_get_priv(dev);
	struct spi_slave *slave = dev_get_parent_priv(dev);
    u32 buswidth;
    int ret;
	slave->max_hz = dev_read_u32_default(dev, "spi-max-frequency", 0);
	slave->mode  |= (dev_read_bool(dev, "spi-cpol") << 1) | dev_read_bool(dev, "spi-cpha");

    buswidth = dev_read_u32_default(dev, "buswidth", 0);
	if (buswidth != 8) {
		dev_err(dev, "Only 8bit buswidth is supported now");
		return -EINVAL;
	}

	ret = gpio_request_by_name(dev, "reset-gpios", 0,
				   &priv->reset_gpio, GPIOD_IS_OUT);
	if (ret) {
		dev_err(dev, "missing reset GPIO\n");
		return ret;
	}

	ret = gpio_request_by_name(dev, "dc-gpios", 0,
				   &priv->dc_gpio, GPIOD_IS_OUT);
	if (ret) {
		dev_err(dev, "missing dc GPIO\n");
		return ret;
	}

	uc_priv->bpix = VIDEO_BPP16;
	uc_priv->xsize = dev_read_u32_default(dev, "width", 0);
	uc_priv->ysize = dev_read_u32_default(dev, "height", 0);
	uc_priv->rot = dev_read_u32_default(dev, "rotate", 0);

	priv->dev = dev;

	ret = spi_vedio_startup(dev);
	if (ret)
		return ret;

	return 0;
}

static int spi_video_bind(struct udevice *dev)
{
	struct video_priv *uc_priv = dev_get_uclass_priv(dev);
	struct video_uc_plat *plat = dev_get_uclass_plat(dev);
	
	plat->size = uc_priv->xsize * uc_priv->ysize * 2;

	return 0;
}

static const struct video_ops spi_video_ops = {
    .video_sync =  spi_video_sync
};

static const struct udevice_id spi_video_ids[] = {
    { .compatible = "sitronix,st7789v" },
    {}
};

U_BOOT_DRIVER(spi_video) = {
    .name   = "spi_video",
    .id     = UCLASS_VIDEO,
    .of_match = spi_video_ids,
    .priv_auto = sizeof(struct spi_video_priv),
	.bind = spi_video_bind,
    .probe = spi_video_probe,
    .ops    = &spi_video_ops,
};