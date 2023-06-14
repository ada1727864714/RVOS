#include "plic.h"

extern void trap_vector(void);
volatile plic_t *const plic = (volatile plic_t *)PLIC_BASE;

void interrupt_vector_init(){
    /*
     * 设置机器模式的interrupt-vector基址，
     * 同时设置中断向量模式
     */
    
    
    int hart=r_tp();
#ifdef K210
    w_mtvec((reg_t)trap_vector);

    plic->source_priorities.priority[UARTHS_IRQ] = 1;

    /* Get current enable bit array by IRQ number */
    uint32_t c1 = plic->target_enables.target[hart].enable[0];
    uint32_t c2 = plic->target_enables.target[hart].enable[1];
    uint32_t c3 = plic->target_enables.target[hart].enable[2];
    plic->target_enables.target[hart].enable[0] = 0;
    plic->target_enables.target[hart].enable[1] = 0;
    plic->target_enables.target[hart].enable[2] = 0;
    uint32_t current = plic->target_enables.target[hart].enable[UARTHS_IRQ / 32];
    /* Set enable bit in enable bit array */
    current |= (uint32_t)1 << (UARTHS_IRQ % 32);
    /* Write back the enable bit array */
    plic->target_enables.target[hart].enable[UARTHS_IRQ / 32] = current;
    uint32_t c4 = plic->target_enables.target[hart].enable[1];

    plic->targets.target[hart].priority_threshold = 0;
#else
    reg_t mtvec_vector_table= (reg_t)&__mtvec_vector_table;
    mtvec_vector_table |= VECTOR_MODE; 
    w_mtvec(mtvec_vector_table);
    /*
     * 设定UART0的优先级 
     * 
     * 每个PLIC中断源可以通过写入其32位内存映射优先级寄存器
     * 来分配优先级。QEMU-virt设备（类似FU540-C000）支持7级优先级。
     * 优先级0表示永不中断，并有效地禁用中断。优先级1为最低的活动
     * 优先级，优先级7为最高的活动优先级。具有相同优先级的全局中断
     * 之间的关系由中断ID判断；ID最小的中断具有最高的优先级。
     */
    *(uint32_t*)PLIC_PRIORITY(UART0_IRQ) = 1;

    /* 
     * 使能UART0
     * 
     * 每个全局中断都可以通过使能enable寄存器相应的位来启用。
     */
    *(uint32_t*)PLIC_MENABLE(hart)= (1 << UART0_IRQ);

    /* 
     * 设置PLIC的优先级阈值
     * 所有小于或者等于（<=）该阈值的中断源即使发生了也会被PLIC丢弃。
     * 特别地，当阈值为0时允许所有中断源上发生的中断；
     * 当阈值为7时丢弃所有中断源上发生的中断。
     */
    *(uint32_t*)PLIC_MTHRESHOLD(hart) = 0;
#endif
    /* enable machine-mode external interrupts. */
	w_mie(r_mie() | MIE_MEIE);

    /* enable machine-mode global interrupts. */
	w_mstatus(r_mstatus() | MSTATUS_MIE);
}

/* 
 * Claim和Complete是同一个寄存器，每个Hart一个.
 * 对该寄存器执行读操作称之为Claim，即获取当前发生的最高优先级的中断源ID。
 * Claim成功后会清除对应的Pending位.
 * 对该寄存器执行写操作称之为Complete。
 * 所谓Complete指的是通知PLIC对该路中断的处理已经结束
 * 写入值为之前的获取值
 */
uint32_t plic_claim(void){
    int hart = r_tp();
#ifdef K210
    return plic->targets.target[hart].claim_complete;
#else
    int irq = *(uint32_t*)PLIC_MCLAIM(hart);
    return irq;
#endif
}

void plic_complete(int irq){
    int hart = r_tp();
#ifdef K210
    plic->targets.target[hart].claim_complete = irq;
#else
    *(uint32_t*)PLIC_MCOMPLETE(hart) = irq;
#endif
}

void default_vector_handler(){
    //留作备用
}

void Machine_timer_handler(){
    //暂未实现
}

reg_t Machine_external_handler(reg_t epc, reg_t cause){
    reg_t return_pc = epc;
	reg_t cause_code = cause & 0xfff;

    int irq = plic_claim();
#ifdef K210
    if(irq == UARTHS_IRQ){
        uart_isr();
        printf("text uart input!!!\n");
    } else if(irq){
        printf("unexpected interrupt irq = %d\n", irq);
    }
#else
    if(irq == UART0_IRQ){
        uart_isr();
        printf("text uart input!!!\n");
    } else if(irq){
        printf("unexpected interrupt irq = %d\n", irq);
    }
#endif
    /* 防止irq为0时也进行完成操作 */
    if(irq){
        plic_complete(irq);
    }

    return return_pc;
}


