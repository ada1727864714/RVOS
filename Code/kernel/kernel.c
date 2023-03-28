#include "../include/os.h"

void start_kernel(void){
    uart_init();
    uart_puts("Hello,RVOS!\n");

    trap_init();

    malloc_init();
    printf("1\n");

    malloc_test();


    //malloc_init();

	sched_init();

	os_main();

	task_yield();
	uart_puts("Would not go here!\n");
    while (1){}; //系统在此空转
}