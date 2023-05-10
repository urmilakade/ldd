obj-m += driver.o

#KDIR = /lib/modules/$(shell uname -r)/build
KDIR = /lib/modules/ 5.4.0-146-generic/build


all:
	 make -C $(KDIR)  M=$(shell pwd) modules

clean:
	make -C $(KDIR)  M=$(shell pwd) clean
