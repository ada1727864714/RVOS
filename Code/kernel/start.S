#include "../platform/qemu_virt.h"

    #每个hart的栈大小为 1024 bytes
    .equ STACK_SIZE,4*1024

    .global _start

    .text
_start:
    #设置hart，仅hart0运行，其它hart休眠
    csrr t0,mhartid     #读取当前hart id
    mv tp,t0            #将hart id保存在tp寄存器中以后使用
    bnez t0,park        #如果当前hart id不为0，进入休眠

    #设置BSS section的所有byte为0
    la a0, _bss_start
    la a1, _bss_end
    bgeu a0,a1, _code_continue #无符号方式比较，a0>=a1时跳转
_set_zero:
    sw zero, (a0)
    addi a0,a0,4
    bltu a0,a1, _set_zero #无符号方式比较，a0<a1时跳转
_code_continue:
    #设置栈，栈是从底部开始生长的，所以我们将栈指针设置到栈底
    slli t0,t0,10       #左移hart id 10位，低10位为1024字节，正好为每个hart的栈空间
    la sp, stacks + STACK_SIZE #设置栈指针指向栈底
    
    add sp,sp,t0        #将栈指针移动到当前hart的栈底

    j start_kernel      #hart 0跳转到c

park:
    wfi                 #RISC-V架构定义的一条休眠指令
    j park

stacks:
    .skip   STACK_SIZE #为所有hart分配空间

    .end           #文件截止

