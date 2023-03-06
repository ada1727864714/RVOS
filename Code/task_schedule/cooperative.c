#include "task_schedule.h"

/* 该函数定义在 entry.S */
extern void switch_to(struct context *next);

/* 定义任务上限 */
#define MAX_TASKS 10
#define STACK_SIZE 1024

/* 定义任务栈和各任务上下文 */
uint8_t task_stack[MAX_TASKS][STACK_SIZE];
struct context ctx_tasks[MAX_TASKS];

/*
 * _top：用于标记ctx_tasks的最大可用位置
 * _current：用于指向当前任务的上下文
 */
static int _top = 0;
static int _current = -1;

/* 写mscratch寄存器 */
static void w_mscratch(reg_t x){
    asm volatile("csrw mscratch,%0": : "r" (x));
}

void sched_init(){
    w_mscratch(0);
}

/*
 * 实现一个简单的循环FIFO调度
 */
void schedule(){
    if(_top <= 0){
        panic("Num of task should be greater than zero!");
        return;
    }

    /* 使用%操作实现一个循环 */
    _current = (_current + 1) % _top;

    /* 切换上下文 */
    struct context *next = &(ctx_tasks[_current]);
    switch_to(next);
}

/* 
 * 描述：
 * 创建一个任务
 * - start_routin：任务例程进入点
 * 返回值：
 * 0：创建成功
 * -1：有错误发生
 */
int task_create(void (*start_routin)(void)){
    if(_top < MAX_TASKS){
        ctx_tasks[_top].sp = (reg_t) &task_stack[_top][STACK_SIZE-1];
        ctx_tasks[_top].ra = (reg_t) start_routin;
        _top++;
        return 0;
    }else{
        return -1;
    }
}

/*
 * 描述：
 * 调用任务放弃CPU，一个新任务开始运行
 */
void task_yield(){
    schedule();
}

/*
 * 描述：
 * 一个粗糙的delay实现，仅用来消耗CPU资源
 */
void task_delay(volatile int count){
    count *= 50000;
    while(count--);
}