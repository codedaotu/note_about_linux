#将.c的生成文件.o作为目标文件（自动识别，不需要写.c）
#obj-m:=module_hello.o 
#obj-m:=task_print.o 
obj-m:=map_driver.o 
CURRENT_PATH := $(shell pwd)#当前路径	
KERNEL_VERSION := $(shell uname -r)#当前内核版本
KERNEL_PAYH := /lib/modules/$(KERNEL_VERSION)/build #当前内核路径（本人换过核，为当前版本的内核路径）
all:  # make前面是tab健
	make -C	${KERNEL_PAYH} M=$(CURRENT_PATH) modules  
#modules表面编译的是内核模块 -C 选项的作用是指将当前工作目录转移到你所指定的位置。\
“M=”选项的作用是，当用户需要以某个内核为基础编译一个外部模块的话，需要在make modules 命令中加入“M=dir”，程序会自动到你所指定的dir目录中查找模块源码，将其编译，生成KO文件。
clean:
	make -C	$(KERNEL_PAYH) M=$(CURRENT_PATH) clean
