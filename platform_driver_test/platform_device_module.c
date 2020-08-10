#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/device.h>

struct platform_device *my_device;

static int __init my_device_init(void)
{
	int ret = 0;	
	my_device=platform_device_alloc("casey_dev",-1);
	ret=platform_device_add(my_device);
	printk("device install \n");
	if(ret)
        	platform_device_put(my_device);
	
	return ret;
}

static void __exit my_device_exit(void)
{
	platform_device_unregister(my_device);
	printk("device uninstall \n");
}

module_init(my_device_init);
module_exit(my_device_exit);

MODULE_AUTHOR("casey");
MODULE_LICENSE("GPL");

