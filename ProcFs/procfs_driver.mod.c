#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/export-internal.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;
BUILD_LTO_INFO;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif


static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0x92997ed8, "_printk" },
	{ 0x88db9f48, "__check_object_size" },
	{ 0x13c49cc2, "_copy_from_user" },
	{ 0x87a21cb3, "__ubsan_handle_out_of_bounds" },
	{ 0x37a0cba, "kfree" },
	{ 0xf301d0c, "kmalloc_caches" },
	{ 0x35789eee, "kmem_cache_alloc_trace" },
	{ 0x6b10bee1, "_copy_to_user" },
	{ 0x7682ba4e, "__copy_overflow" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
	{ 0x4800abdd, "cdev_init" },
	{ 0x58c36c14, "cdev_add" },
	{ 0xaee657ee, "__class_create" },
	{ 0xf3e489db, "device_create" },
	{ 0x91e77b10, "proc_mkdir" },
	{ 0x3f1584d8, "proc_create" },
	{ 0x645620c0, "class_destroy" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0xc23e0fb8, "proc_remove" },
	{ 0xbdee1658, "device_destroy" },
	{ 0x8bee73ff, "cdev_del" },
	{ 0x541a6db8, "module_layout" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "0B0590C7E7110C90D716BEB");
