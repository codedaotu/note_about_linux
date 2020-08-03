#include<linux/init.h>
#include<linux/kernel.h>// printk
#include<linux/module.h>
/*
__init 用于初始化的修饰符，告诉编译程序，执行完后对内存进行回收
*/
static int __init hello_init(void)//模块的入口函数，模块加载时执行
{
    printk("hello world \n");
    return 0;
}
module_init(hello_init);//初始化模块,在module.h内

static void __exit hello_exit(void)//模块的出口函数，模块卸载时执行
{
    printk("hello world exit\n");
}
module_exit(hello_exit);//模块卸载

MODULE_AUTHOR("CASEY");
MODULE_LICENSE("GPL V2");
MODULE_DESCRIPTION("hello world module");
MODULE_ALIAS("hello module");//模块别名,没啥用？