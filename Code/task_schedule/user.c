#include "../include/os.h"

#define DELAY 1000

void user_task0(void)
{
	uart_puts("Task 0: Created!\n");
	int i = 0;
	while (i < 5) {
		uart_puts("Task 0: Running...\n");
		trap_test();
		i++;
		task_delay(DELAY);
		task_yield();
	}
	task_exit();
}

void user_task1(void)
{
	uart_puts("Task 1: Created!\n");
	int i = 0;
	while (i < 10) {
		uart_puts("Task 1: Running...\n");
		i++;
		task_delay(DELAY);
		task_yield();
	}
	task_exit();
}

void user_task2(void)
{
	uart_puts("Task 2: Created!\n");
	int i = 0;
	while (i < 10) {
		uart_puts("Task 2: Running...\n");
		i++;
		task_delay(DELAY);
		task_yield();
	}
	task_exit();
}

/* NOTICE: DON'T LOOP INFINITELY IN main() */
void os_main(void)
{
	task_create(user_task0,NULL,0);
	task_create(user_task1,NULL,0);
	task_create(user_task2,NULL,1);
}