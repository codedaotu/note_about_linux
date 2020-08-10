#include <linux/acct.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/device.h>
#include <linux/module.h>
static int my_probe(struct platform_device *dev)
{
	printk("my_probe running \n");
	return 0;
}

static int my_remove(struct platform_device *dev)
{
	printk("my_remove running  \n");
	return 2;
}
struct platform_driver my_driver={
	.probe = my_probe,
	.remove = my_remove,
	.driver = {
	.name="casey_dev",
	}
};

static int __init my_driver_init(void)
{
	int ret=0;
	ret=platform_driver_register(&my_driver);
	printk("driver install \n");
	return 0;
}	
static void __exit my_driver_exit(void)
{
	platform_driver_unregister(&my_driver);
	printk("driver uninstall \n");
}
module_init(my_driver_init);
module_exit(my_driver_exit);

MODULE_AUTHOR("casey");
MODULE_LICENSE("GPL");
//MODULE_ALIAS("platform:casey_dev1");
//MODULE_ALIAS("UBS:casey_dev");
MODULE_ALIAS("platform:casey_dev");

