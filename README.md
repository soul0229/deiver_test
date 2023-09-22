# test
> #### devicetree
```
&spi1  {
    status = "okay";
    pinctrl-names = "default";
    pinctrl-0 = <&spi1_pins>, <&spi1_cs0_pin>;
    st7789v: st7789v@0{
        compatible = "sitronix,st7789v";
        reg = <0>;
        status = "okay";
        spi-max-frequency = <100000000>;
        spi-cpol;
        spi-cpha;
        rotate = <0>;
        fps = <60>;
        buswidth = <8>;
        rgb;
        led-gpios   = <&pio 2 6 GPIO_ACTIVE_HIGH>;   // PC6
        dc-gpios    = <&pio 2 7 GPIO_ACTIVE_HIGH>;  // PC7 
        reset-gpios = <&pio 2 8 GPIO_ACTIVE_LOW>;  // PC8
        width = <240>;
        height= <240>;
        debug = <0>;
    };
};
```
