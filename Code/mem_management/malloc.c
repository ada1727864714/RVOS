#include "malloc.h"

/* _num_sizes */
static uint32_t _num_sizes = 0;

/* 设置一个链表头 */
static struct Block *first_block = NULL;

void malloc_init(){
    /* 设置堆内存 */
    _num_sizes = (uint32_t)HEAP_SIZE;

    /* 初始化第一个空闲块，该块大小是(HEAP_SIZE - block_head) */
    first_block = (struct Block*)HEAP_START;
    first_block->front = NULL;
    first_block->next = NULL;
    first_block->size_flag = 0;

    _free_flag(first_block);
    _set_size(first_block, _num_sizes - block_head);

    /* 更新_num_sizes */
    _num_sizes = _num_sizes - block_head;

    printf("TEXT:   0x%x -> 0x%x\n", TEXT_START, TEXT_END);
	printf("RODATA: 0x%x -> 0x%x\n", RODATA_START, RODATA_END);
	printf("DATA:   0x%x -> 0x%x\n", DATA_START, DATA_END);
	printf("BSS:    0x%x -> 0x%x\n", BSS_START, BSS_END);
    printf("HEAP:   0x%x -> 0x%x\n", HEAP_START, HEAP_START + HEAP_SIZE);
    printf("\n");
}

/*
 * 分配一个连续的内存块，大小为 size
 * - size：要分配的内存块大小
 */
void *malloc(size_t size){

    struct Block *block = first_block;

    while (block){
        if(_is_free(block)){
            /* tmp获得当前空闲块的可分配大小 */
            uint32_t tmp = (block->size_flag >> 1);
            if(size <= tmp){
                _set_flag(block);
                if((tmp - size) <= block_head){
                    /* 
                     * 当前空闲块的可分配大小减去需要的大小不足或等于block_head字节，
                     * 即剩余空间不可继续分配时，
                     * 将当前空闲块全部分配给请求者，
                     * 此时不需要创建新的链表项。
                     */
                    _set_size(block,tmp);
                    printf("block_head: 0x%x\n",block);
                    printf("block_size: %d\n",block->size_flag >> 1);
                    /* 因为要返回给用户实际使用的地址，所以 +1 跳过链表头 */
                    return (block + 1);
                }else{
                    /* 
                     * 当剩余空间还可分配时，
                     * 创建新的链表项
                     */
                    _set_size(block,size);
                    struct Block *new_block = (struct Block*)((void *)block + size + block_head);
                    _clear(new_block);
                    new_block->front = block;
                    new_block->next = block->next;
                    block->next = new_block;
                    _free_flag(new_block);
                    _set_size(new_block ,(tmp - size - block_head));
                    printf("block_head: 0x%x\n",block);
                    printf("block_size: %d\n",block->size_flag >> 1);
                    /* 因为要返回给用户实际使用的地址，所以 +1 跳过链表头 */
                    return (block + 1);
                }
            }
        }
        block = block->next;
    }
    printf("当前堆无足够大小的块，无法分配\n");
    return NULL;
}

/*
 * 释放内存块，同时合并相邻的空闲块
 * - ptr：内存块可分配部分的起始地址
 */
void free(void *ptr){
    /*
     * Assert (TBD) if p is invalid
     */
    if(!ptr || ptr >=(HEAP_START + HEAP_SIZE)){
        return;
    }

    /* 获得该块的描述符 */
    printf("\n");
    struct Block *block = (struct Block*)(ptr - 0xc);
    printf("block: 0x%x\n",block);
    struct Block *front_block = block->front;
    printf("front_block: 0x%x\n",front_block);
    struct Block *next_block = block->next;
    printf("next_block: 0x%x\n",next_block);
    printf("\n");
    
    /* 首先查看后一块内存是否空闲，空闲即合并，再查看前一块 */
    if(_is_free(next_block)){
        block->next = next_block->next;
        uint32_t block_size = (block->size_flag >> 1);
        uint32_t next_block_size = (next_block->size_flag >> 1);
        printf("block_size: %d ,next_block_size: %d\n",block_size,next_block_size);
        _set_size(block ,(block_size + next_block_size + block_head));
        _clear(next_block);
        _free_flag(block);

        uint32_t new_block_size = block->size_flag >> 1;
        printf("block = 0x%x, block->next = 0x%x, new_block_size = %d\n", block, block->next, new_block_size);
    }
    if(_is_free(front_block)){
        front_block->next = block->next;
        uint32_t block_size = (block->size_flag >> 1);
        uint32_t front_block_size = (front_block->size_flag >> 1);
        printf("front_block_size: %d ,block_size: %d\n",front_block_size,block_size);
        _set_size(front_block,(front_block_size + block_size + block_head));
        _clear(block);
        _free_flag(front_block);
        
        uint32_t new_front_block_size = front_block->size_flag >> 1;
        printf("front_block = 0x%x, front_block->next = 0x%x, new_front_block_size = %d\n", front_block, front_block->next, new_front_block_size);
    }else{
        _free_flag(block);
    }
    printf("\n");
}

void malloc_test(){
    void *p = malloc(1024);
    printf("p = 0x%x\n", p);

    void *p1 = malloc(512);
    printf("p1 = 0x%x\n", p1);

    void *p2 = malloc(256);
    printf("p2 = 0x%x\n", p2);

    void *p3 = malloc(256);
    printf("p3 = 0x%x\n", p3);

    void *p4 = malloc(128);
    printf("p4 = 0x%x\n", p4);

    free(p1);
    free(p3);
    free(p2);

    void *p5 = malloc(1040);
    printf("p5 = 0x%x\n", p5);
}



