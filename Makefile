PWD := $(shell pwd)
KERNEL := /home/ntai/Linux_Emb/kernelbuildscripts/KERNEL
CROSS := /home/ntai/Linux_Emb/kernelbuildscripts/dl/gcc-8.5.0-nolibc/arm-linux-gnueabi/bin/arm-linux-gnueabi-
obj-m += lcd_i2c.o

all:
	make ARCH=arm CROSS_COMPILE=$(CROSS) -C $(KERNEL) M=$(PWD) modules
clean:
	make -C $(KERNEL) M=$(PWD) clean

copy:
	scp lcd_i2c.ko debian@192.168.7.2:/home/debian

