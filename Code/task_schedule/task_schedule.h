#ifndef __TASK_SCHEDULE_H__
#define __TASK_SCHEDULE_H__

#include "../include/os.h"
#include <stddef.h> 

/* 该函数定义在 entry.S */
extern void switch_to(struct context *next);

/* 定义优先级等级上限 */
#define Priority_num 10
/* 定义任务栈大小 */
#define STACK_SIZE 1024

/* task_num：保存当前系统中的任务总数 */
extern int task_num;

extern void exit();
extern void schedule();

/* task management */
struct context {
	/* ignore x0 */
	reg_t ra;
	reg_t sp;
	reg_t gp;
	reg_t tp;
	reg_t t0;
	reg_t t1;
	reg_t t2;
	reg_t s0;
	reg_t s1;
	reg_t a0;
	reg_t a1;
	reg_t a2;
	reg_t a3;
	reg_t a4;
	reg_t a5;
	reg_t a6;
	reg_t a7;
	reg_t s2;
	reg_t s3;
	reg_t s4;
	reg_t s5;
	reg_t s6;
	reg_t s7;
	reg_t s8;
	reg_t s9;
	reg_t s10;
	reg_t s11;
	reg_t t3;
	reg_t t4;
	reg_t t5;
	reg_t t6;
};

#endif