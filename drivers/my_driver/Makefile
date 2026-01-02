obj-m += my_driver.o

KDIR := /home/pc/Documents/Advanced_Embedded/linux

all:
	make -C $(KDIR) M=$(PWD) all

modules:
	make -C $(KDIR) M=$(PWD) modules

clean:
	make -C $(KDIR) M=$(PWD) clean
