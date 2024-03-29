#include "page.h"

/*
 * _alloc_start：指向堆池的实际起始地址
 * _alloc_end：指向堆池的实际结束地址
 * _num_pages：保存我们可以分配的实际最大页面数
 */
static reg_t _alloc_start = 0;
static reg_t _alloc_end = 0;
static reg_t _num_pages = 0;

void page_init(){
    /*
     * 我们需要8页（8 × 4096）来存放Page结构体
     * 这足够来管理 128MB（8 × 4096 × 4096）的heap空间
     */
    _num_pages = (HEAP_SIZE / PAGE_SIZE) -8;
    printf("HEAP_START = %x, HEAP_SIZE = %x, num of pages = %d\n", HEAP_START, HEAP_SIZE, _num_pages);
    
    /* 
     * HEAP_START可以当做一个指针，他的值就是一个地址
     * (struct Page*)这步强转其实是规定了后面page++的步长（这也是强转的一大含义）
     * 此时page++一次前进一个字节，_clear的清空其实无所谓填入page
     * 它只是接受一个地址，然后清空这一个字节
     * 所以，你甚至可以使用 (char *) 对HEAP_START进行强转
     * 所以此时 i<_num_pages 就变得合理，以为我们仅需要管理这部分页
     */
    struct Page *page = (struct Page*)HEAP_START;
    for(int i=0; i < _num_pages; i++){
        _clear(page);
        page++;
    }

    /* 真正分配的堆先进行对齐，加快内存访问速度 */
    _alloc_start = _align_page(HEAP_START + 8 * PAGE_SIZE);
    _alloc_end = _alloc_start + (PAGE_SIZE * _num_pages);

    printf("TEXT:   0x%x -> 0x%x\n", TEXT_START, TEXT_END);
	printf("RODATA: 0x%x -> 0x%x\n", RODATA_START, RODATA_END);
	printf("DATA:   0x%x -> 0x%x\n", DATA_START, DATA_END);
	printf("BSS:    0x%x -> 0x%x\n", BSS_START, BSS_END);
	printf("HEAP:   0x%x -> 0x%x\n", _alloc_start, _alloc_end);
}

/*
 * 分配一个由连续物理内存页组成的内存块
 * - npages：要分配的内存页数
 */
void *page_alloc(int npages){

    /* 首先，我们通过也页面数组搜索第一块空闲的页 */
    int found = 0;
    struct Page *page_i = (struct Page*)HEAP_START;

    for(int i = 0; i <= (_num_pages - npages); i++){
        if(_is_free(page_i)){
            found = 1;
            /*
             * 当遇到空闲页时，搜索空闲页后的
             * (npages-1)个页面是否也空闲
             */
            struct Page *page_j = page_i + 1;
            for(int j = i + 1; j < (i+npages); j++){
                if(!_is_free(page_j)){
                    /* 如果发现这片页有被占用无法组成内存块，就放弃 */
                    found = 0;
                    break;
                }
                page_j++;
            }
            /*
             * 当获得一个足够的内存块时，
             * 进行必要的内务管理，然后返回
             * 这个内存块第一页的实际起始地址
             */
            if(found){
                struct Page *page_k = page_i;
                /* 将该内存块的每一页设置为已使用 */
                for(int k = i; k < (i + npages); k++){
                    _set_flag(page_k, PAGE_TAKEN);
                    page_k++;
                }
                /* 设置内存块的最后一页为最后一页 */
                page_k--;
                _set_flag(page_k,PAGE_LAST);
                return (void *)(_alloc_start + i * PAGE_SIZE);
            }
        }
        page_i++;
    }    
    return NULL;
}

/*
 * 释放内存块
 * - p：内存块的起始地址
 */
void page_free(void *p){
    /*
     * Assert (TBD) if p is invalid
     */
    if(!p || (reg_t)p >= _alloc_end){
        return;
    }
    /* 获得该内存块的第一页描述符 */
    struct Page *page = (struct Page *)HEAP_START;
    page += ((reg_t)p - _alloc_start)/ PAGE_SIZE;
    /* 循环并清除内存块的所有页面描述符 */
    while (!_is_free(page)){
        if(_is_last(page)){
            _clear(page);
            break;
        }else{
            _clear(page);
            page++;
        }
    }   
}

/* 进行一些测试 */
void page_test(){
    void *p = page_alloc(2);
	printf("p = 0x%x\n", p);
	//page_free(p);

	void *p2 = page_alloc(7);
	printf("p2 = 0x%x\n", p2);
	page_free(p2);

	void *p3 = page_alloc(4);
	printf("p3 = 0x%x\n", p3);
}