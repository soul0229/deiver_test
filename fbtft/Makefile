obj-m += mytftfb.o
KDIR:=/home/wish/H616/linux
PWD?=$(shell pwd)
all:
	make -C $(KDIR) M=$(PWD) modules
	echo $(PWD)
clean:
	rm -f *.ko *.mod.c *.symvers *.order
