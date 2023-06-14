#include "../include/os.h"

/* 异常处理函数 */
/* K210无中断向量模式，故直接进入该函数 */
reg_t trap_handler(reg_t epc, reg_t cause){
    
    reg_t return_pc = epc;
    /* mcause寄存器的低12位足够代表trap类型 */
    
    reg_t cause_code = cause & 0xfff;
    if (cause & 0x8000000000000000) {
		/* Asynchronous trap - interrupt */
		switch (cause_code) {
		case 3:
			uart_puts("software interruption!\n");
			break;
		case 7:
			uart_puts("timer interruption!\n");
			break;
		case 11:
			uart_puts("external interruption!\n");
			Machine_external_handler(epc,cause);
			break;
		default:
			uart_puts("unknown async exception!\n");
			break;
		}
	} else {
		/* Synchronous trap - exception */
		printf("Sync exceptions!, code = %ld\n", cause_code);
        printf("%lx\n",cause);
		panic("OOPS! What can I do!");
		//return_pc += 4;
	}

    return return_pc;
}

/* 进行异常测试 */
void trap_test(){
    /*
	 * Synchronous exception code = 7
	 * Store/AMO access fault
	 */
    *(int *)0x0000000000000003 = 100;

    /*
	 * Synchronous exception code = 5
	 * Load access fault
	 */
	// int a = *(int *)0x00000000;

    uart_puts("Yeah! I'm return back fron trap!\n");
}

