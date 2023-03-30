#include "../include/os.h"

extern void trap_vector(void);

void trap_init(){
    /*
     * 设置机器模式的trap-vector基址
     */
    w_mtvec((reg_t)trap_vector);
}

/* 异常处理函数 */
reg_t trap_handler(reg_t epc, reg_t cause){
    
    reg_t return_pc = epc;
    /* mcause寄存器的低12位足够代表trap类型 */
    reg_t cause_code = cause & 0xfff;

    /* 
     * 根据mcause寄存器的定义，当最高位为1时，
     * 是中断，所以先进行相关判断
     */
    if(cause & 0x80000000){
        /* 中断处理逻辑 */
        switch (cause_code)
        {
        case 3:
            uart_puts("software interruption!\n");
            break;
        case 7:
            uart_puts("timer interruption!\n");
            break;
        case 11:
            uart_puts("external interruption!\n");
            break;
        default:
            uart_puts("unknown async exception!\n");
            break;
        }
    }else{
        /* 异常处理逻辑 */
        printf("Sync exceptions!, code = %d\n", cause_code);
        printf("%ld\n",cause);
        panic("OOPS! What can I do!");
    }

    return return_pc;
}

/* 进行异常测试 */
void trap_test(){
    /*
	 * Synchronous exception code = 7
	 * Store/AMO access fault
	 */
    printf("1\n");
    *(int *)0x00000000 = 100;

    /*
	 * Synchronous exception code = 5
	 * Load access fault
	 */
	//int a = *(int *)0x00000000;

    uart_puts("Yeah! I'm return back fron trap!\n");
}

