#include "../include/os.h"

/*
 * 下面的两个函数只调用一次，所以仅在此声明不包含在 os.h中
 */
extern void uart_init(void);
extern void page_init(void);
extern void page_test(void);
extern void block_init(void);
extern void malloc_test(void);

void start_kernel(void){
    uart_init();
    uart_puts("Hello,RVOS!\n");

    block_init();

    malloc_test();

    while (1){}; //系统在此空转
}