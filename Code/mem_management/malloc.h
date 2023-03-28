#ifndef __MALLOC_H__
#define __MALLOC_H__

#include "../include/os.h"

/*
 * 下列全局变量定义在mem.S中
 */
extern reg_t TEXT_START;
extern reg_t TEXT_END;
extern reg_t DATA_START;
extern reg_t DATA_END;
extern reg_t RODATA_START;
extern reg_t RODATA_END;
extern reg_t BSS_START;
extern reg_t BSS_END;
extern reg_t HEAP_START;
extern reg_t HEAP_SIZE;

/* 定义链表头大小 */
#define block_head 24

/* 块头最后一个字节表示占用情况，为 1 占用 */
#define BLOCK_TAKEN (reg_t)(1 << 0)

/*
 * Block描述：
 * Block是堆分配块的信息结构体，占用block_head个字节大小
 * front：前向指针，指向前一个内存块
 * next：后向指针，指向后一个内存块
 * size_flag：标志位，共32位，其中最后一位记录该块是否被占用
 * 其余位记录该块大小
 */
struct Block{
    reg_t *front;
    reg_t *next;
    reg_t size_flag; 
};

/* 清空块头 */
static inline void _clear(struct Block *block)
{
    block->front = NULL;
    block->next = NULL;
    block->size_flag = 0;
}

/* 如果该块被占用，返回 0，否则返回 1 */
static inline int _is_free(struct Block *block)
{
    if(block->size_flag & BLOCK_TAKEN){
        return 0;
    }else{
        return 1;
    }
}

/* 设置标志位，表示该块被占用 */
static inline void _set_flag(struct Block *block){
    block->size_flag |= BLOCK_TAKEN;
}

/* 设置标志位，表示该块空闲 */
static inline void _free_flag(struct Block *block){
    block->size_flag &= ~BLOCK_TAKEN;
}

/* 设置块大小 */
static inline void _set_size(struct Block *block, reg_t size)
{
    reg_t flag = _is_free(block);
    reg_t tmp = size << 1;
    block->size_flag = tmp;
    if(flag){
        _free_flag(block);
    }else{
        _set_flag(block);
    }
}

#endif