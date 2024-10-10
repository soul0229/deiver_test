# vedio driver use spi lcd
## spi_vedio devicetree
```
&pinctrl {
	spi-vedio {
		lcd_pin: lcd-pin{
			rockchip,pins =

				<3 RK_PA1 0 &pcfg_pull_up_drv_level_1>,

				<3 RK_PA2 0 &pcfg_pull_up_drv_level_1>,

				<3 RK_PA3 0 &pcfg_pull_up_drv_level_1>;
		};
	};
};

&spi3 {
	pinctrl-0 = <&spi3m1_cs0_hs &spi3m1_cs1_hs &spi3m1_pins_hs>, <&lcd_pin>;
	status = "okay";
	st7789v: st7789v@0{
        compatible = "sitronix,st7789v";
        reg = <0>;
        status = "okay";
		spi-max-frequency = <100000000>;
        spi-cpol = <0>;
        spi-cpha = <1>;
        rotate = <0>;
        buswidth = <8>;
        dc-gpios    = <&gpio3 RK_PA2 GPIO_ACTIVE_HIGH>;  
        reset-gpios = <&gpio3 RK_PA3 GPIO_ACTIVE_HIGH>;  
        width = <240>;
        height= <240>;
    };
};
```