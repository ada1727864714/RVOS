/*
 * 设置相关类型，使其适应32位系统
 */

#ifndef __TYPES_H__
#define __TYPRS_H__

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;

/*
 * RISCV64: register is 64bits width
 */ 
typedef uint64_t reg_t;

#endif /* __TYPES_H */