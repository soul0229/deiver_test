obj-m += a.o
KDIR:=/usr/src/linux-headers-5.16.17-sun50iw9
PWD?=$(shell pwd)
all:
	make -C $(KDIR) M=$(PWD) modules
	echo $(PWD)
clean:
	rm -f *.ko *.mod.c *.symvers *.order
