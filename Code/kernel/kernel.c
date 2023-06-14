#include "../include/os.h"

void start_kernel(void){
    uart_init();
    uart_puts("Hello,RVOS!\n");

    // page_init();
    // page_test();
    sched_init();
    interrupt_vector_init();
    malloc_init();

    malloc_test();

	// //sched_init();

	os_main();

	task_yield();
	uart_puts("Would not go here!\n");
    while (1){}; //系统在此空转
}