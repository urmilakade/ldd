#include<linux/module.h>
#include<linux/fs.h>
#include<linux/cdev.h>
#include<linux/slab.h>                 //kmalloc()
#include<linux/uaccess.h>              //copy_to/from_user()
#include <linux/err.h>
#include <linux/kernel.h>
#include<linux/sysfs.h>
#include<linux/kobject.h>

#define DEV_MEM_SIZE 512
#define mem_size        1024           //Memory Size

/* buffer for kernel*/
uint8_t *kernel_buffer;
volatile char  value[10] = {0};
struct kobject *kobj_ref;

/*pseudo devices imemory*/
char device_buffer[DEV_MEM_SIZE];

/*this module hols device number*/
dev_t device_number;

/*Cdev variable*/
static struct class *dev_class;
struct cdev pcd_cdev;

/******************file operation of device*************************/

static int      file_open(struct inode *inode, struct file *file);
static int      file_release(struct inode *inode, struct file *file);
static ssize_t  file_read(struct file *filp, char __user *buf, size_t len,loff_t * off);
static ssize_t  file_write(struct file *filp, const char *buf, size_t len, loff_t * off);
static struct file_operations pcd_fops =
{
    .owner      = THIS_MODULE,
    .read       = file_read,
    .write      = file_write,
    .open       = file_open,
    .release    = file_release,
};
/*************** Sysfs functions **********************/
static ssize_t  sysfs_show(struct kobject *kobj,struct kobj_attribute *attr, char *buf);
static ssize_t  sysfs_store(struct kobject *kobj,struct kobj_attribute *attr,const char *buf, size_t count);

//struct kobj_attribute etx_attr = __ATTR(sys_file,0666, sysfs_show, sysfs_store); // make file error
//struct kobj_attribute etx_attr = __ATTR(sys_file,(S_IRWXG | S_IRWXU), sysfs_show, sysfs_store);//data not append 
//struct kobj_attribute etx_attr = __ATTR(sys_file,(S_IRWXG | S_IRWXU | O_APPEND), sysfs_show, sysfs_store); // make file issue
struct kobj_attribute etx_attr = __ATTR(sys_file,(0660 & O_APPEND), sysfs_show, sysfs_store); // Permission denied



/*
** This function will be called when we read the sysfs file
*/
static ssize_t sysfs_show(struct kobject *kobj,struct kobj_attribute *attr, char *buf)
{
        pr_info("Sysfs - Read!!!\n");
        return sprintf(buf, "%s", value);
}

/*
** This function will be called when we write the sysfsfs file
*/
static ssize_t sysfs_store(struct kobject *kobj, struct kobj_attribute *attr,const char *buf, size_t count)
{
        pr_info("Sysfs - Write!!!\n");
        sscanf(buf,"%s",value);
        return count;
}
/*_________________________________________________________________________________________________*/
/*
** This function will be called when we open the Device file
*/
static int file_open(struct inode *inode, struct file *file)
{
	 /*Creating Physical memory*/
        if((kernel_buffer = kmalloc(mem_size , GFP_KERNEL)) == 0){
            pr_info("Cannot allocate memory in kernel\n");
	    return -1;
	}
	else
	{
        	pr_info("Driver Open Function Called...!!!\n");
       		 return 0;
	}
}
/*
** This function will be called when we close the Device file
*/
static int file_release(struct inode *inode, struct file *file)
{
	kfree(kernel_buffer);
        pr_info("Driver Release Function Called...!!!\n");
        return 0;
}
/*
** This function will be called when we read the Device file
*/
static ssize_t file_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{  
	//Copy the data from the kernel space to the user-space
        if( copy_to_user(buf, kernel_buffer, len ) )
        {
                pr_err("Data Read : Err!\n");
		return 0;
        }
	else
	{
        	pr_info("Driver Read Function Called...!!!\n");
   	        return len ;
	}
}
/*
** This function will be called when we write the Device file
*/
static ssize_t file_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
	 //Copy the data to kernel space from the user-space
        if( copy_from_user(kernel_buffer, buf, len) )
        {
                pr_err("Data Write : Err!\n");
		return 0;
        }
	else
	{
       		pr_info("Driver Write Function Called...!!!\n");
        	return len;
	}
}




static int __init my_char_driver_init(void)
{
	/*1. dynamically allocate a device number*/
	alloc_chrdev_region(&device_number, 0 , 2, "char_driver");

	/*2. Initialize cdev struct with fops*/
	cdev_init(&pcd_cdev, &pcd_fops);

	/*3. Register a device (cdev) structure with VFS*/
	pcd_cdev.owner = THIS_MODULE;
	cdev_add(&pcd_cdev, device_number, 1);

        /*Creating struct class*/
        if(IS_ERR(dev_class = class_create(THIS_MODULE,"sys_class"))){
            pr_err("Cannot create the struct class\n");
            goto r_class;
        }
        /*Creating device*/
        if(IS_ERR(device_create(dev_class,NULL,device_number,NULL,"sys_device"))){
            pr_err("Cannot create the Device \n");
            goto r_device;
        }
	  /*Creating a directory in /sys/kernel/ */
        kobj_ref = kobject_create_and_add("sysfs_char",kernel_kobj);

        /*Creating sysfs file for value*/
        if(sysfs_create_file(kobj_ref,&etx_attr.attr)){
                pr_err("Cannot create sysfs file......\n");
                goto r_sysfs;
	}
        pr_info("char Driver Insert...Done!!!\n");
        return 0;

r_sysfs:
        kobject_put(kobj_ref);
        sysfs_remove_file(kernel_kobj, &etx_attr.attr);

r_device:
        class_destroy(dev_class);
r_class:
        unregister_chrdev_region(device_number,1);
        return -1;

}

static void __exit  my_char_driver_exit(void)
{
	 kobject_put(kobj_ref);
        sysfs_remove_file(kernel_kobj, &etx_attr.attr);
        device_destroy(dev_class,device_number);
        class_destroy(dev_class);
        cdev_del(&pcd_cdev);
        unregister_chrdev_region(device_number, 1);
        pr_info("Char  Driver Remove...Done!!!\n");

}


module_init(my_char_driver_init);
module_exit(my_char_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("URMILA");
MODULE_DESCRIPTION("A pseudo char driver");
//MODULE_("");
