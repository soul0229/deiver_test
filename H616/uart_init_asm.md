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