#include "../include/os.h"

void start_kernel(void){
    uart_init();
    uart_puts("Hello,RVOS!\n");
/*
    malloc_init();

    malloc_test();
*/

    page_init();

	sched_init();

	os_main();

	schedule();

	uart_puts("Would not go here!\n");
    while (1){}; //系统在此空转
}