#include<linux/module.h>
#include<linux/fs.h>
#include<linux/cdev.h>
#include<linux/slab.h>                 //kmalloc()
#include<linux/uaccess.h>              //copy_to/from_user()
#include <linux/err.h>
#include <linux/kernel.h>

#define DEV_MEM_SIZE 512
#define mem_size        1024           //Memory Size

/* buffer for kernel*/
uint8_t *kernel_buffer;

/*pseudo devices imemory*/
char device_buffer[DEV_MEM_SIZE];

/*this module hols device number*/
dev_t device_number;

/*Cdev variable*/
static struct class *dev_class;
struct cdev pcd_cdev;

/*file operation of device*/

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
        if(IS_ERR(dev_class = class_create(THIS_MODULE,"char_class"))){
            pr_err("Cannot create the struct class\n");
            goto r_class;
        }
        /*Creating device*/
        if(IS_ERR(device_create(dev_class,NULL,device_number,NULL,"char_device"))){
            pr_err("Cannot create the Device 1\n");
            goto r_device;
        }
	pr_info("Char driver insert\n");
        return 0;


r_device:
        class_destroy(dev_class);
r_class:
        unregister_chrdev_region(device_number,1);
        return -1;

}

static void __exit  my_char_driver_exit(void)
{

	device_destroy(dev_class,device_number);
        class_destroy(dev_class);
        cdev_del(&pcd_cdev);
        unregister_chrdev_region(device_number, 1);
	pr_info("Char driver remove\n");
}


module_init(my_char_driver_init);
module_exit(my_char_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("URMILA");
MODULE_DESCRIPTION("A pseudo char driver");
//MODULE_("");
