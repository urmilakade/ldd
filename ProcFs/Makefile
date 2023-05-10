#obj-m := heelo_world.o


#obj-m := module_param.o

#obj-m := my_char_driver.o
obj-m := procfs_driver.o

KDIR = /lib/modules/$(shell uname -r)/build 
#iHOST = /lib/modules/$(shell uname -r)/build/ 
#KDIR = /lib/modules/5.10.0-051000-generic/build 
#KDIR = /lib/modules/5.4.0-146-generic/build 
HOST = /lib/modules/5.10.0-051000-generic/build/ 

all:
	make -C $(KDIR)  M=$(PWD) modules
 
clean:
	make -C $(KDIR)  M=$(PWD) clean
host:
	make -C $(HOST)  M=$(PWD) modules
