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
> #### uart_init
```
.globl	_uart_init
_uart_init:
	ldr x4,=0x0300190c
	ldr w5,=0x00010001
	str w5,[x4]

	ldr x4,=0x0300b0fc
	ldr w5,=0x00000022
	str w5,[x4]

	ldr x4,=0x0300b118
	ldr w5,=0x00000005
	str w5,[x4]

	ldr x4,=0x05000008
	ldr w5,=0x00000001
	str w5,[x4]

	ldr x4,=0x050000A4
	ldr w5,=0x00000001
	str w5,[x4]

	ldr x4,=0x0500000c
	ldr w5,=0x00000083
	str w5,[x4]

	ldr x4,=0x05000000
	ldr w5,=0x0000000d
	str w5,[x4]

	ldr x4,=0x0500000c
	ldr w5,=0x00000003
	str w5,[x4]

	ldr x4,=0x050000A4
	ldr w5,=0x00000000
	str w5,[x4]

	ldr x4,=0x05000008
	ldr w5,=0x00000000
	str w5,[x4]
	ldr w5,=0x00000001
	str w5,[x4]

	ldr x4,=0x05000000
	ldr w5,=0x00000075
	str w5,[x4]				/* "u" */

	ldr w5,=0x00000061
	str w5,[x4]				/* "a" */

	ldr w5,=0x00000072
	str w5,[x4]				/* "r" */

	ldr w5,=0x00000074
	str w5,[x4]				/* "t" */

	ldr w5,=0x0000006f
	str w5,[x4]				/* "o" */

	ldr w5,=0x0000006b
	str w5,[x4]				/* "k" */

	ldr w5,=0x00000021
	str w5,[x4]				/* "!" */

	ldr w5,=0x0000000d
	str w5,[x4]				/* "\n" */

	ret
.globl	display_hex
display_hex:
	ldr x4,=0x05000000

	ldr x5,=0x30
	str w5,[x4]				/* "0" */

	ldr x5,=0x78
	str w5,[x4]

	ldr x6,=60
display_loop_cnt:
	lsrv x5,x0,x6
	and x5,x5,#0xf
	cmp x5,#10
	add x5,x5,#0x30
	blt 1f
	add x5,x5,#0x7
1:
	str w5,[x4]

	sub x6,x6,#4
	cmp x6,#0
	bge display_loop_cnt

	ldr x5,=0xd
	str w5,[x4]

	ret
```
> # ts视频合成.bat
```
@echo off
setlocal enabledelayedexpansion

set /a output=1
set "target_folder=%~dp0"
dir /B /TC /O:D /AD > folders_order.txt

REM 遍历当前文件夹下的所有一级子文件夹
for /F "delims=" %%G in (folders_order.txt) do (
REM for /D %%G in (*) do (
    echo folder1: %%G
	set /a file_count=1000
	REM 遍历文件夹中的每个.ts文件
	cd %target_folder%\%%G
	for /R %%A in (.) do (
		echo folder2: %%A
		for %%F in ("%%A\*.ts") do (
			REM 移动文件到目标文件夹
			move "%%F" "%target_folder%\%%G"
		)
	)
	set /a count=1000
	set /a i=0
	REM 遍历当前文件夹下的所有.ts文件
	for %%F in (*.ts) do (
		echo Processing file: !i!
		REM 重命名文件
		ren "!i!.ts" "(!count!).ts"
		REM 增加计数器的值
		echo count = !count!
		echo i = !i!
		set /a count+=1
		set /a i+=1
	)
	copy /b "*.ts" "%target_folder%\!output!.ts"
	cd ..
	rd /s /q "%target_folder%\%%G"
	set /a output+=1
)

for %%F in ("%target_folder%\*.*") do (
    set "extension=%%~xF"
    if /i "!extension!" neq ".ts" (
        if /i "!extension!" neq ".bat" (
            if /i "!extension!" neq ".txt" (
				del "%%F"
			)
        )
    )
)
echo succeed
endlocal
pause
```
