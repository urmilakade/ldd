#obj-m := heelo_world.o


#obj-m := module_param.o

obj-m := my_char_driver.o

KDIR = /lib/modules/$(shell uname -r)/build 
HOST = /lib/modules/$(shell uname -r)/build/ 

all:
	make -C $(KDIR)  M=$(PWD) modules
 
clean:
	make -C $(KDIR)  M=$(PWD) clean
host:
	make -C $(HOST)  M=$(PWD) modules
