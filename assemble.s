	.file	"assembler_c.c"    gcc 留下的文件信息
	.text						标识下面为代码段
	.globl	a					函数a的说明
	.type	a, @function
a:								函数a的入口地址
.LFB0:							码中有许多以.开头的代码行，属于链接时候的辅助信息,在实际中不会执行，把它删除，	
	.cfi_startproc				函数开始的符号
.LFB0:								
	endbr64                     该指令不运行，用于检测目标地址是要跳转的类型
	pushq	%rbp  				压栈rbp保存，rbp为栈基址寄存器---指向栈底
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16     
	movq	%rsp, %rbp			将rsp传给rbp   rsp 栈指针寄存器，指向栈顶
	.cfi_def_cfa_register 6
	movl	%edi, -4(%rbp)  	  edi为变址寄存器(Index Register)，它们主要用于存放存储单元在段内的偏移量
	movl	-4(%rbp), %eax        EAX寄存器以称为累加器
	addl	$3, %eax
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	a, .-a
	.globl	b
	.type	b, @function
b:
.LFB1:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$8, %rsp
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %eax
	movl	%eax, %edi
	call	a
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE1:
	.size	b, .-b
	.globl	main
	.type	main, @function
main:                     eip可存储着cpu要读取指令的地址，没有了它，cpu就无法读取下面的指令，每次相应汇编指令执行完相应的eip值就会增加。
.LFB2:
	.cfi_startproc
	endbr64
	pushq	%rbp           rbp 栈底，rsp 栈顶，rbp进栈
	.cfi_def_cfa_offset 16  
	.cfi_offset 6, -16
	movq	%rsp, %rbp     rsp->rbp
	.cfi_def_cfa_register 6
	pushq	%rbx            rbx 通用寄存器
	.cfi_offset 3, -24
	movl	$1, %edi        edi=1
	call	a               调用a
	movl	%eax, %ebx
	movl	$1, %edi
	call	b
	addl	%ebx, %eax
	addl	$1, %eax
	popq	%rbx
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE2:
	.size	main, .-main
	.ident	"GCC: (Ubuntu 9.3.0-10ubuntu2) 9.3.0    ·"
	.section	.note.GNU-stack,"",@progbits
	.section	.note.gnu.property,"a"
	.align 8
	.long	 1f - 0f
	.long	 4f - 1f
	.long	 5
0:
	.string	 "GNU"
1:
	.align 8
	.long	 0xc0000002
	.long	 3f - 2f
2:
	.long	 0x3
3:
	.align 8
4:


#include<stdio.h>
//analyse c
 int a(int x)
 {
     return x+3;
 }

 int b(int x)
 {
     return a(x);
 }

int main() {
     return a(1)+b(1)+1;
 }