/*本程序用与熟悉task_struct结构体（linux/sched.h）
 *version 1
 *author  casey
 *date 2020.08.04
 */
#include<linux/init.h>  
#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/sched.h>
#include<linux/fdtable.h>
#include<linux/fs_struct.h>
#include<linux/mm_types.h>
#include<linux/init_task.h>
#include<linux/types.h>
#include<linux/atmioc.h>
/*
init.h 包含了模块初始化/注销的定义 module_init()/exit()
module.h 写内核驱动必包含的头文件。常用的宏定义如 MODULE_LICESENCE(),MODULE_AUTHOR(),等在此文件中
kernel.h 有: printk
sched.h task_struct
fs_struct.h	fs	fs_struct	filesystem information 
fdtable.h	files	file_struct	Open file information	
mm_types.h	mm->total_vm	mm_struct	total_vm为进程的地址空间的大小
init_task.h	init_task变量
types.h		用到了原子变量
atomic.h	atomic_read	
*/


static int __init task_print_init(void)
{
	struct task_struct *task,*p;
	struct list_head *pos;
	int count_process = 0;
	task = &init_task;
	list_for_each(pos,&task->tasks)
	{
		p=list_entry(pos,struct task_struct,tasks);
		count_process++;
		printk("\n\n");
		printk("pid %d;state %lx;prio %d;static_prio %d; parient pid %d;files count %d;umask %d\n",\
		p->pid,p->state,p->prio,p->static_prio,(p->parent)->pid\
		,atomic_read(&(p->files)->count),(p->fs)->umask);
		if ((p->mm)!=NULL)
			printk("total_vm %ld \n",(p->mm)->total_vm);
	}
	printk("all process: %d \n",count_process);
	return 0;
}

/* 打印的信息有：
 * pid 
 * state	进程的状态信息
 * prio		动态优先级
 * static_prio  静态优先级
 * ppid		父进程
 * umask	
 * count
 * */
static void __exit task_print_exit(void)
{
	printk("task_print_exit \n");
}
module_init(task_print_init);
module_exit(task_print_exit);

MODULE_AUTHOR("casey");
MODULE_LICENSE("GPL V2");
MODULE_DESCRIPTION("TASK PRINT");
MODULE_ALIAS("TASK_PRINT");

