/*
 * 定义OS相关函数
 */

#ifndef __OS_H__
#define __OS_H__

#include "types.h"
#include "platform.h"

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
extern int uart_putc(char ch);
extern void uart_puts(char *s);

/* printf */
extern int printf(const char* s, ...);
extern void panic(char *s);

/* page级内存管理方法 */
extern void *page_alloc(int npages);
extern void page_free(void *p);

/* malloc级内存管理方法 */
extern void *malloc(size_t size);
extern void free(void *ptr);

#endif /* __OS_H__ */
