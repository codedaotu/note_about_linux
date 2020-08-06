/*本程序用于监视系统负载
 *version 1
 *author casey
 *date  2020.08.05
 */
#include<linux/version.h>
#include<linux/module.h>
#include<linux/hrtimer.h>
#include<linux/ktime.h>
#include<linux/kallsyms.h>
#include<linux/sched.h>
#include<linux/tracepoint.h>
#include<linux/sched/task.h>
#include<linux/sched/signal.h>
#include<linux/stacktrace.h>

#define FSHIFT		11
#define FIXED_1		(1<<FSHIFT)
#define LOAD_INT(x)	((x) >> FSHIFT)
#define LOAD_FRAC(x)	LOAD_INT(((x)&(FIXED_1-1)) * 100)
#define BACKTRACE_DEPTH	20

struct hrtimer timer;
static unsigned long *ptr_avenrun;
struct stack_trace {
	unsigned int nr_entries, max_entries;
	unsigned long *entries;
	int skip;	/* input argument: How many entries to skip */
};
static void print_all_task_stack(void)
{
	struct task_struct *g,*p;
	unsigned long backtrace[BACKTRACE_DEPTH];
//	memset(&trace,0,sizeof(trace));

	memset(backtrace,0,BACKTRACE_DEPTH*sizeof(unsigned long));
	//trace.max_entries = BACKTRACE_DEPTH;
	//trace.entries = backtrace;
	printk("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
	printk("\t load：%lu.%02lu,%lu.%02lu,%lu.%02lu\n",
		LOAD_INT(ptr_avenrun[0]),LOAD_FRAC(ptr_avenrun[0]),
		LOAD_INT(ptr_avenrun[1]),LOAD_FRAC(ptr_avenrun[1]),
		LOAD_INT(ptr_avenrun[2]),LOAD_FRAC(ptr_avenrun[2]));
	printk("dump all task : \n");
	rcu_read_lock();
	/*
	 read_copy_update 锁，因为在读进程时，要便利进程链表，有些进程会被清理或者杀死，所以要在遍历时加锁。
	 读者不需要获得锁就可以访问，
	 但是写者在访问时要拷贝一个副本，对副本修改，最后使用回调机制对数据进行修改，
	 将原数据指针指向修改后的数据
	 */
	printk("dump running task : \n");
	do_each_thread(g,p){
		if(p->state == TASK_RUNNING){
			printk("runnning task ,comm:%s,pid %d\n",p->comm,p->pid);
		//	memset(&trace,0,sizeof(trace));
			memset(backtrace,0,BACKTRACE_DEPTH*sizeof(unsigned long));
		//	trace.max_entries = BACKTRACE_DEPTH;
		//	trace.entries = backtrace;
		//	save_stack_trace_tsk(p,&trace);
		//	print_stack_trace(&trace,0);
		}
	}while_each_thread(g,p);
/*
 do_each_thread 和while_each_thread是两个宏，用于便利线程。
 do_each_thread	将0号进程的task_struct赋值给g,p.根据进程的父子关系遍历系统中所有的进程（线程），满足处在
 running状态下的进程，将其堆栈的数据保存在trace中，并且打印出来。
 每次打印都需要初始化数组backtrace,和结构体trace
*/
	printk("dump uninterupted task. \n");
	do_each_thread(g,p){
		if(p->state & TASK_UNINTERRUPTIBLE){
		printk("uninterrupted task ,comm:%s,pid %d\n",
			p->comm,p->pid);
	//	memset(&trace,0,sizeof(trace));
		memset(backtrace,0,BACKTRACE_DEPTH*sizeof(unsigned long));
	//	trace.max_entries = BACKTRACE_DEPTH;
	//	trace.entries = backtrace;
	//	save_stack_trace_tsk(p,&trace);
	//	print_stack_trace(&trace,0);
		}
	}while_each_thread(g,p);
	rcu_read_unlock();
}
static void check_load(void)
{
	static ktime_t last;
	u64 ms;
	int load = LOAD_INT(ptr_avenrun[0]);/*1min的load值*/
	if (load<0)
		return;
	/*上次打印时间与当前相差不到20s,退出*/
	ms=ktime_to_ms(ktime_sub(ktime_get(),last));
	if (ms <20 *1000)
		return;
	last = ktime_get();//获取当前的系统时间
	print_all_task_stack();//打印所有的线程站
}
static enum hrtimer_restart moniter_handler( struct hrtimer *hrtimer)
{
	enum hrtimer_restart  ret = HRTIMER_RESTART;
	check_load();
	hrtimer_forward_now(hrtimer,ms_to_ktime(10));
	return ret;
}
/*
返回定时器重启的信号，枚举类型有两个值，RESTART和NORESTART。
check_load 是用于检查系统负载值的。
*/
static void start_timer(void)
{
	hrtimer_init(&timer,CLOCK_MONOTONIC,HRTIMER_MODE_PINNED);
	timer.function = moniter_handler;
	hrtimer_start_range_ns(&timer,ms_to_ktime(10),0,HRTIMER_MODE_REL_PINNED);
}
/*
 moniter_handler 是到时后执行的回调函数，回调函数是指 使用者自己定义一个函数，实现这个函数的程序内容，然后把这
 个函数（入口地址）作为参数传入别人（或系统）的函数中，由别人（或系统）的函数在运行时来调用的函数。
 hrtimer_start_range_ns 是倒计时重启，以毫秒计时，此处为10ms。
 */
static int __init load_monitor_init(void)
{
	ptr_avenrun = (void *)kallsyms_lookup_name("avenrun");
	if(!ptr_avenrun)
		return -EINVAL;
	start_timer();
	printk("load-monitor loaded \n");
	return 0;	
}
/*
 *avenrun是内核定义的无符号长整形的数组，有三个元素，因为
 *内核不能很好的支持浮点型，低11位存放的是小数部分，高位存放整数 *部分
 *top uptime 都是读取的这个数组,某些版本的内核没有导出这个变量，利用kallsy这个
 *函ptr_avenrun得变量地址，将其赋给指针prt_avenrun
 **/
static void __exit load_monitor_exit(void)
{
	hrtimer_cancel(&timer);
	printk("load-moniter exited \n");

}
module_init(load_monitor_init);
module_exit(load_monitor_exit);

MODULE_AUTHOR("casey");
MODULE_LICENSE("GPL");
