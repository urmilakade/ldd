#include<linux/module.h>
#include<linux/fs.h>
#include<linux/cdev.h>
#include<linux/slab.h>                 //kmalloc()
#include<linux/uaccess.h>              //copy_to/from_user()
#include <linux/err.h>
#include <linux/kernel.h>
#include<linux/proc_fs.h>


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

char etx_array[20]= {0};
static struct proc_dir_entry *parent;
static int len = 1,count = 0;

/*************** Driver Functions **********************/
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
	{	int i;
		for(i =0 ; kernel_buffer[i] ; i++)
			etx_array[i] = kernel_buffer[i];
		etx_array[i] = '\0';

		pr_info("Driver Write Function Called...!!!\n");
		return len;
	}
}
/***************** Procfs Functions *******************/
static int      open_proc(struct inode *inode, struct file *file);
static int      release_proc(struct inode *inode, struct file *file);
static ssize_t  read_proc(struct file *filp, char __user *buffer, size_t length,loff_t * offset);
static ssize_t  write_proc(struct file *filp, const char *buff, size_t len, loff_t * off);
loff_t          lseek_proc(struct file *filp, loff_t  off,int whence);
/*
 ** procfs operation sturcture
 */
/*//static struct file_operations proc_fops = {
  static struct proc_ops proc_fops= {
  .open = open_proc,
  .read = read_proc,
  .write = write_proc,
  .release = release_proc,
  .lseek = lseek_proc
  };*/
static struct proc_ops proc_fops = {
	.proc_open = open_proc,
	.proc_read = read_proc,
	.proc_write = write_proc,
	.proc_release = release_proc,
	.proc_lseek = lseek_proc
};

loff_t  lseek_proc(struct file *filp, loff_t  off,int whence)
{
	if( whence == SEEK_SET)
		filp->f_pos = off;
	else if (whence == SEEK_CUR)
		filp->f_pos = filp->f_pos + off;
	else if (whence ==  SEEK_END)
		filp->f_pos = DEV_MEM_SIZE + off;
	return 	filp->f_pos;
}
/*
 ** This function will be called when we open the procfs file
 */
static int open_proc(struct inode *inode, struct file *file)
{
	file->f_flags = (file->f_flags & O_APPEND);
	printk(KERN_INFO "proc file opend.....\t");
	return 0;
}
/*
 ** This function will be called when we close the procfs file
 */
static int release_proc(struct inode *inode, struct file *file)
{
	printk(KERN_INFO "proc file released.....\n");
	return 0;
}
/*
 ** This function will be called when we write the procfs file
 */
static ssize_t write_proc(struct file *filp, const char *buff, size_t len, loff_t * off)
{

	if( copy_from_user(etx_array,buff,len) )
	{
		pr_err("Data Write : Err!\n");
		return 0;
	}
	else
	{
		lseek_proc(filp,len, SEEK_CUR);
		pr_info("proc file wrote..... off = %lld filp->f_pos = %lld\n",*off,filp->f_pos);
	}
	return len;
}
/*
 ** This function will be called when we read the procfs file
 */
static ssize_t read_proc(struct file *filp, char __user *buffer, size_t length,loff_t * offset)
{
	if(len)
	{
		len=0;
	}
	else
	{
		len=1;
		return 0;
	}
	for(count = 0; etx_array[count];count++);
	if (count != length)
		count = length;


	if( copy_to_user(buffer,etx_array,20) )
	{
		pr_err("Data Send : Err!\n");
		return 0;
	}
	else
	{
		lseek_proc(filp,length, SEEK_CUR);
	//	*offset = filp->f_pos;
		pr_info("proc file read..... off = %lld filp->f_pos = %lld\n",*offset,filp->f_pos);
		//printk(KERN_INFO "proc file read.....\n");
	}

	return length;;
}



static int __init my_char_driver_init(void)
{
	/*1. dynamically allocate a device number*/
	alloc_chrdev_region(&device_number, 0 , 2, "char_driver");
	pr_info("Major = %d Minor = %d \n",MAJOR(device_number), MINOR(device_number));


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

	/*Create proc directory. It will create a directory under "/proc" */
	parent = proc_mkdir("char_driver",NULL);

	if( parent == NULL )
	{
		pr_info("Error creating proc entry");
		goto r_device;
	}

	/*Creating Proc entry under "/proc/char_driver/" */
	//proc_create("char_proc", 0666, parent, &proc_fops);
	proc_create("char_proc", (0555 | O_APPEND), parent, &proc_fops);


	pr_info("!!_______Char driver insert_____!!\n");
	return 0;
r_device:
	class_destroy(dev_class);
r_class:
	unregister_chrdev_region(device_number,1);
	return -1;



}

static void __exit  my_char_driver_exit(void)
{
	/* remove complete /proc/etx */
	proc_remove(parent);

	device_destroy(dev_class,device_number);
	class_destroy(dev_class);
	cdev_del(&pcd_cdev);
	unregister_chrdev_region(device_number, 1);

	pr_info("!!_____Char driver remove_____!!\n");
}


module_init(my_char_driver_init);
module_exit(my_char_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("URMILA");
MODULE_DESCRIPTION("A pseudo char driver");
//MODULE_("");
