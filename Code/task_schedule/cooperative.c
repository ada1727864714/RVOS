#include "task_schedule.h"

/* 定义每一个任务的结构体 */
struct Task
{
	uint8_t task_stack[STACK_SIZE];
	uint8_t priority;
	struct context ctx_tasks;
	struct Task *front;
	struct Task *next;
};


/* 定义schedule函数自己的上下文和栈空间 */
struct context schedule_context;
uint8_t schedule_stack[STACK_SIZE];

/* 定义exit函数自己的上下文和栈空间 */
struct context exit_context;
uint8_t exit_stack[STACK_SIZE];

/* 定义管理任务的优先级数组 */
struct Task task_priority_array[Priority_num];

/*
 * now_priority：指定当前的任务优先级
 * now_task：指向当前任务的结构体
 */
static uint8_t now_priority; 
static struct Task *now_task;

/* task_num：保存当前系统中的任务总数 */
int task_num = 0;

/* schedule初始化 */
void sched_init(){
    /* 设置mscratch寄存器初值 */
    w_mscratch(0);
    /* 初始化任务优先级数组 */
    for(int i = 0;i < Priority_num;i++){
        task_priority_array[i].next = NULL;
    }
    
    /* 设置schedule函数的上下文 */
    schedule_context.sp = &schedule_stack[STACK_SIZE - 1];
    schedule_context.ra = &schedule;

    /* 设置exit函数的上下文 */
    exit_context.sp = &exit_stack[STACK_SIZE - 1];
    exit_context.ra = &exit;

    /* 初始化now_priority和now_task指针 */
    now_priority = 0;
    now_task = NULL;
}

/*
 * 描述：
 * 作为系统调用的接口，调用任务放弃CPU，一个新任务开始运行
 */
void task_yield(){
    switch_to(&schedule_context);
}

/*
 * 实现任务调度
 */
void schedule(){
    /* 获取当前正在执行的任务 */  
    struct Task *task = now_task;
    /* 指向当前任务数字里最高优先级的第一个任务 */
    struct Task *first_task = NULL;

    /* 获取第一个任务 */
    for(int i = 0;i < Priority_num;i++){
        if(task_priority_array[i].next != NULL){
            first_task = task_priority_array[i].next;
            break;
        }
    }
    /* 若无任务，则退出schedule */
    if(first_task == NULL){
        panic("No task to schedule!\n");
        return;
    }
    /* 如果当前任务被删除或第一次调度，则调度搜索到的第一个任务 */
    if(task == NULL){
        now_task = first_task;
        struct context *next = &now_task->ctx_tasks; 
        switch_to(next);
    }else{
        /* 如果当前最高优先级和当前任务的优先级一样，在此任务链表中循环 */
        if(first_task->priority == now_priority){
            now_task = task->next;
            struct context *next = &now_task->ctx_tasks; 
            switch_to(next);
        }else{
            /* 否则执行当前最高优先级的任务队列 */
            now_task = first_task;
            struct context *next = &now_task->ctx_tasks; 
            switch_to(next);
        }
    }    
}

/* 
 * 描述：
 * 创建一个任务
 * - task：任务例程进入点
 * 返回值：
 * 0：创建成功
 * -1：有错误发生
 */
int task_create(void (*task)(void* param),void* param,uint8_t priority){
    /* 首先保证任务的优先级不高于或等于当前系统的优先级数量 */
    if(priority < Priority_num){
        /* 如果当前优先级的链表为空，则创建该优先级的第一个任务 */
        if(task_priority_array[priority].next == NULL){
            struct Task *new_task = (struct Task*)malloc(sizeof(struct Task));
            new_task->ctx_tasks.sp = (reg_t) &new_task->task_stack[STACK_SIZE-1];
            new_task->ctx_tasks.ra = (reg_t) task;
            new_task->ctx_tasks.a0 = (reg_t) param;
            new_task->priority = priority;
            new_task->next = new_task;
            new_task->front = new_task;
            task_priority_array[priority].next = new_task;
            task_num++;
        }else{
            struct Task *first_task = task_priority_array[priority].next;
            struct Task *new_task = (struct Task*)malloc(sizeof(struct Task));
            new_task->ctx_tasks.sp = (reg_t) &new_task->task_stack[STACK_SIZE-1];
            new_task->ctx_tasks.ra = (reg_t) task;
            new_task->ctx_tasks.a0 = (reg_t) param;
            new_task->priority = priority;
            new_task->front = first_task->front;
            first_task->front->next = new_task;
            new_task->next = first_task;
            first_task->front = new_task;
            task_priority_array[priority].next = new_task;
            task_num++;
        }
        return 0;
    }else{
        return -1;
    }
}

/* 提供函数退出接口 */
void task_exit(){
    switch_to(&exit_context);
}

void exit(){
    struct Task *task = now_task;
    /* 如果当前链表上只有一个任务，直接清空当前优先级，同时返回 */
    if(task->next == task){
        task_priority_array[task->priority].next = NULL;
        free(task);
        now_task = NULL;
        task_num--;
        switch_to(&schedule_context);
        return;
    }
    /* 如果该任务是当前优先级的进入节点，则切换当前优先级进入节点 */
    if(task_priority_array[task->priority].next == task){
        task_priority_array[task->priority].next = task->next;
    }
    task->front->next = task->next;
    task->next->front = task->front;
    free(task);
    now_task = NULL;
    task_num--;
    switch_to(&schedule_context);
    return;
}

/*
 * 描述：
 * 一个粗糙的delay实现，仅用来消耗CPU资源
 */
void task_delay(volatile int count){
    count *= 50000;
    while(count--);
}