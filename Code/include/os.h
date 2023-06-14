/*
 * 定义OS相关函数
 */

#ifndef __OS_H__
#define __OS_H__

#include "types.h"
#include "../platform/qemu_virt.h"
#include "../platform/K210.h"
#include "riscv.h"

/*
 * stddef.h 头文件定义了各种变量类型和宏，如size_t,NULL等
 * 可参考：https://www.runoob.com/cprogramming/c-standard-library-stddef-h.html
 * 
 * stdarg.h 头文件的主要目的是为了让函数能够接受可变参数
 * 可参考：https://blog.csdn.net/qq_23827747/article/details/79688446
 * 或：https://www.runoob.com/cprogramming/c-standard-library-stdarg-h.html
 */
#include <stddef.h> 
#include <stdarg.h> 

/* uart */
extern void uart_init(void);
extern void uarths_init(void);
extern int k210_uart_init();
extern int uart_putc(char ch);
extern void uart_puts(char *s);

/* printf */
extern int printf(const char* s, ...);
extern void panic(char *s);

/* page级内存管理方法 */
extern void page_init(void);
extern void page_test(void);
extern void *page_alloc(int npages);
extern void page_free(void *p);


/* 协作式任务调度 */
extern int  task_create(void (*task)(void* param),void* param,uint8_t priority);
extern void task_delay(volatile int count);
extern void task_yield();
extern void task_exit();

extern void os_main(void);
extern void sched_init(void);

/* 异常处理相关 */
extern void interrupt_vector_init();
extern reg_t trap_handler(reg_t epc, reg_t cause);
extern void trap_test();

#endif /* __OS_H__ */
