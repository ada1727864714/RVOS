#include "malloc.h"

#pragma pack (8)
/* _num_sizes */
static reg_t _num_sizes = 0;

/* 设置一个链表头 */
static struct Block *first_block = NULL;
#pragma pack ()

void malloc_init(){
    /* 设置堆内存 */
    _num_sizes = (reg_t)HEAP_SIZE;

    /* 初始化第一个空闲块，该块大小是(HEAP_SIZE - block_head) */
    first_block = (struct Block*)HEAP_START;


    first_block->next = NULL;
    first_block->size_flag = 0;
    _free_flag(first_block);
    _set_size(first_block, _num_sizes - block_head);

    /* 更新_num_sizes */
    _num_sizes = _num_sizes - block_head;
    printf("num_sizes:   %ld\n",_num_sizes);

    printf("TEXT:   0x%lx -> 0x%lx\n", TEXT_START, TEXT_END);
	printf("RODATA: 0x%lx -> 0x%lx\n", RODATA_START, RODATA_END);
	printf("DATA:   0x%lx -> 0x%lx\n", DATA_START, DATA_END);
	printf("BSS:    0x%lx -> 0x%lx\n", BSS_START, BSS_END);
    printf("HEAP:   0x%lx -> 0x%lx\n", HEAP_START, HEAP_START + HEAP_SIZE);
    printf("\n");
}

/*
 * 分配一个连续的内存块，大小为 size
 * - size：要分配的内存块大小
 */
void *malloc(size_t size){

    /* 为了保证分配的空间都是8字节对齐的，对未满8字节的空间进行补齐 */
    if (size%8 !=0)
    {
        size += (8 - (size%8));    
    }
    

    struct Block *block = first_block;

    while (block){
        if(_is_free(block)){
            /* tmp获得当前空闲块的可分配大小 */
            reg_t tmp = (block->size_flag >> 1);
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
                    printf("block_head: 0x%lx\n",block);
                    printf("block_size: %ld\n",block->size_flag >> 1);
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
                    printf("block_head: 0x%lx\n",block);
                    printf("block_size: %ld\n",block->size_flag >> 1);
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
    struct Block *block = (struct Block*)(ptr - block_head);
    printf("block: 0x%lx\n",block);
    struct Block *front_block = block->front;
    printf("front_block: 0x%lx\n",front_block);
    struct Block *next_block = block->next;
    printf("next_block: 0x%lx\n",next_block);
    printf("\n");
    
    /* 首先查看后一块内存是否空闲，空闲即合并，再查看前一块 */
    if(_is_free(next_block)){
        block->next = next_block->next;

        /* 如果下一个空闲块不为空，则需将其的前向指针指向前一个块 */
        if(block->next != NULL){
            struct Block *next_next_block = next_block->next;
            next_next_block->front = block;
        }

        reg_t block_size = (block->size_flag >> 1);
        reg_t next_block_size = (next_block->size_flag >> 1);
        printf("block_size: %ld ,next_block_size: %ld\n",block_size,next_block_size);
        _set_size(block ,(block_size + next_block_size + block_head));
        _clear(next_block);
        _free_flag(block);

        reg_t new_block_size = block->size_flag >> 1;
        printf("block = 0x%lx, block->next = 0x%lx, new_block_size = %ld\n", block, block->next, new_block_size);
    }
    if(front_block == NULL){
        _free_flag(block);
        printf("\n");
        return;
    }else if(_is_free(front_block)){
        front_block->next = block->next;

        /* 如果下一个空闲块不为空，则需将其的前向指针指向前一个块 */
        if(block->next != NULL){
            struct Block *next_next_block = block->next;
            next_next_block->front = front_block;
        }

        reg_t block_size = (block->size_flag >> 1);
        reg_t front_block_size = (front_block->size_flag >> 1);
        printf("front_block_size: %ld ,block_size: %ld\n",front_block_size,block_size);
        _set_size(front_block,(front_block_size + block_size + block_head));
        _clear(block);
        _free_flag(front_block);
        
        reg_t new_front_block_size = front_block->size_flag >> 1;
        printf("front_block = 0x%lx, front_block->next = 0x%lx, new_front_block_size = %ld\n", front_block, front_block->next, new_front_block_size);
    }else{
        _free_flag(block);
    }
    printf("\n");
}

void malloc_test(){
    void *p = malloc(1024);
    printf("p = 0x%lx\n", p);

    void *p1 = malloc(512);
    printf("p1 = 0x%lx\n", p1);

    void *p2 = malloc(256);
    printf("p2 = 0x%lx\n", p2);

    void *p3 = malloc(256);
    printf("p3 = 0x%lx\n", p3);

    void *p4 = malloc(128);
    printf("p4 = 0x%lx\n", p4);

    free(p1);
    free(p3);
    free(p2);

    void *p5 = malloc(1040);
    printf("p5 = 0x%lx\n", p5);
}



