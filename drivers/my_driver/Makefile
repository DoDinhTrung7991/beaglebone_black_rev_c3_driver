# Example Makefile snippet
obj-m += my_driver_mod.o
my_driver_mod-objs := my_driver.o std_util.o timer.o

KDIR := /home/pc/Documents/Advanced_Embedded/linux

all:
	make -C $(KDIR) M=$(PWD) all

modules:
	make -C $(KDIR) M=$(PWD) modules

clean:
	make -C $(KDIR) M=$(PWD) clean
