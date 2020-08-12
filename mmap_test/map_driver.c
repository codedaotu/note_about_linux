/*本程序用于探究mmap映射
 *version 1
 *author casey
 *date  2020.08.12
 */
#include <linux/kernel.h>  	
#include <linux/module.h>  
#include <linux/fs.h>  //file_operations
#include <linux/string.h>  
#include <linux/errno.h>  
#include <linux/mm.h>  
#include <linux/vmalloc.h>  
#include <linux/slab.h>  
#include <linux/sched.h>  
#include <asm/io.h>  
#include <linux/mman.h>  

#define MAP_PAGE_COUNT 10  
#define MAPLEN (PAGE_SIZE*MAP_PAGE_COUNT)  
#define MAP_DEV_MAJOR 240
#define MAP_DEV_NAME "mappage"

extern struct mm_struct init_mm;  
void map_vopen(struct vm_area_struct *vma);
void map_vclose(struct vm_area_struct *vma);  
/*device mmap */  
static int mapdrv_mmap(struct file *file, struct vm_area_struct *vma);  
static int mapdrv_open(struct inode *inode, struct file *file); 
/* vm area nopage 
内核5.4.53的返回值为 vm_fault_t   原型为typedef __bitwise unsigned int vm_fault_t;
*/  
vm_fault_t map_fault(struct vm_fault *vmf);  
/*
 owner 它是一个指向拥有这个结构的模块的指针.这个成员用来在它的操作还在被使用时阻止模块被卸载. 几乎所有时间中, 它被简单初始化为 
 THIS_MODULE, 一个在 中定义的宏.这个宏比较复杂，在进行简单学习操作的时候，一般初始化为THIS_MODULE。
 mmap 用来请求将设备内存映射到进程的地址空间. 如果这个方法是 NULL, mmap 系统调用返回 -ENODEV.
 open  打开设备  
*/
static struct file_operations mapdrvo_fops = {  
    
    .owner = THIS_MODULE,  
    .mmap = mapdrv_mmap,  
    .open = mapdrv_open,
}; 

static struct vm_operations_struct map_vm_ops = {
    .open = map_vopen,
    .close = map_vclose,
    .fault = map_fault,
};
   
 
static char *vmalloc_area = NULL;  

/*
 * 装载函数作用
 * 1 设备注册
 * 2 给注册的设备分配内存
 * */
static int __init mapdrv_init(void)  
{  
   int result;
   unsigned long virt_addr;
   int i = 1;
                        //主设备号，设备名称，以及设备链接的file_operation结构
    //如果主设备号为0，则自动分配一个，传递给result。
   result=register_chrdev(MAP_DEV_MAJOR,MAP_DEV_NAME,&mapdrvo_fops);
   if(result<0){
	   return result;
   }
   /*
   vmalloc 分配的是虚拟地址连续，物理地址不连续，并给这块虚拟地址空间申请物理内存(基本是不连续的)，并写入页表。
   在此处分配了十页
   */
   vmalloc_area=vmalloc(MAPLEN);
   virt_addr = (unsigned long)vmalloc_area;
   for(virt_addr = (unsigned long)vmalloc_area; virt_addr < (unsigned long)vmalloc_area + MAPLEN; virt_addr += PAGE_SIZE)
   {
       //对所分配的页的标志位进行设定，表明该页已被占用
	    SetPageReserved(vmalloc_to_page((void *)virt_addr));   
        sprintf((char *)virt_addr, "test %d",i++);             
   }
   /* printk("vmalloc_area at 0x%lx (phys 0x%lx)\n",(unsigned long)vmalloc_area,(unsigned long)vmalloc_to_pfn((void *)vmalloc_area) << PAGE_SHIFT);  */
   printk("vmalloc area apply complate!");
    return 0;
}  
  
static void __exit mapdrv_exit(void)  
{  
    unsigned long virt_addr;  
    /* unreserve all pages */  
    for(virt_addr = (unsigned long)vmalloc_area; virt_addr < (    unsigned long)vmalloc_area + MAPLEN; virt_addr += PAGE_SIZE) 
    {  //对所占用的页进行回收，将标志位清除
        ClearPageReserved(vmalloc_to_page((void *)virt_addr));  
    }  
    /* 回收虚拟内存 */  
    if (vmalloc_area)
        vfree(vmalloc_area);  
    //注销设备号
    unregister_chrdev(MAP_DEV_MAJOR,MAP_DEV_NAME);
  
}  
  

  
static int mapdrv_mmap(struct file *file, struct vm_area_struct *vma)  
{  
    unsigned long offset = vma->vm_pgoff << PAGE_SHIFT;  
    unsigned long size = vma->vm_end - vma->vm_start;  
   
    if (size > MAPLEN) {  
        printk("size too big\n");  
        return -ENXIO;  
    }  
    /*  only support shared mappings. */  
    if ((vma->vm_flags & VM_WRITE) && !(vma->vm_flags & VM_SHARED)) {  
        printk("writeable mappings must be shared, rejecting\n");  
        return -EINVAL;  
    }  
    /* do not want to have this area swapped out, lock it */  
    vma->vm_flags |= VM_LOCKONFAULT;  
    if (offset == 0) {  
        vma->vm_ops = &map_vm_ops;   
    } else {  
        printk("offset out of range\n");  
        return -ENXIO;  
    }  
    return 0;  
}
static int mapdrv_open(struct inode *inoe, struct file *file)
{

    printk("process: %s (%d)\n", current->comm, current->pid);
    return 0;
}  
  
/* open handler for vm area */  
void map_vopen(struct vm_area_struct *vma)  
{  
    printk("mapping vma is opened..\n");
}  
  
/* close handler form vm area */  
void map_vclose(struct vm_area_struct *vma)  
{  
    printk("mapping vma is closed..\n");
}  
  
/* page fault handler */ 

vm_fault_t map_fault(struct vm_fault *vmf)  
{  
	struct page *page;
	void *page_ptr;
        unsigned long offset, virt_start, pfn_start;	
        offset = vmf->address-vmf->vma->vm_start;
        virt_start = (unsigned long)vmalloc_area + (unsigned long)(vmf->pgoff << PAGE_SHIFT);
        pfn_start = (unsigned long)vmalloc_to_pfn((void *)virt_start);

	printk("\n");    
	/*printk("%-25s %d\n","7)PAGE_SHIFT",PAGE_SHIFT);*/
	page_ptr=NULL;
	if((vmf->vma==NULL)||(vmalloc_area==NULL)){
		printk("return VM_FAULT_SIGBUS!\n");
		return VM_FAULT_SIGBUS;
	}
	if(offset >=MAPLEN){
		printk("return VM_FAULT_SIGBUS!");
		return VM_FAULT_SIGBUS;
	}
	page_ptr=vmalloc_area + offset;
	page=vmalloc_to_page(page_ptr);
	get_page(page);	
	vmf->page=page; 
    //%016共占16个字符宽度，如果被输出的数不足16位，则前面用0填充
        printk("%s: map 0x%lx (0x%016lx) to 0x%lx , size: 0x%lx, page:%ld \n", __func__, virt_start, pfn_start << PAGE_SHIFT, vmf->address,PAGE_SIZE,vmf->pgoff);

	return 0;
}
module_init(mapdrv_init);
module_exit(mapdrv_exit);
MODULE_AUTHOR("casey");
MODULE_LICENSE("GPL"); 

