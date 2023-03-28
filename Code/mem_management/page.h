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

/* 每一页的大小是 4KB 即 4096B */
#define PAGE_SIZE 4096
/* 所以偏移量为 12，因为4096是2^12 */
#define PAGE_ORDER 12

#define PAGE_TAKEN (uint8_t)(1<<0)
#define PAGE_LAST  (uint8_t)(1<<1)

/*
 * Page描述：
 * flages:
 * - bit 0：标识该页是否已分配
 * - bit 1：标识该页是否为内存块的最后一页
 */
struct Page{
    uint8_t flages;
};

/* 清空页标志位 */
static inline void _clear(struct Page *page)
{
    page->flages = 0;    
};

/* 如果该页被占用，返回 0，否则返回 1 */
static inline int _is_free(struct Page *page)
{
    if(page->flages & PAGE_TAKEN){
        return 0;
    }else{
        return 1;
    }
};

/* 设置标志位 */
static inline void _set_flag(struct Page *page,uint8_t flags)
{
    page->flages |= flags;
};

/* 如果该页是内存块最后一页，返回 1，否则返回 0 */
static inline int _is_last(struct Page *page)
{
    if(page->flages & PAGE_LAST){
        return 1;
    }else{
        return 0;
    }
};

/*
 * 将地址与页边框对齐（4K）
 * 在后文代码中我们可以看出，该函数主要用处在
 * 当heap前边放置完内存管理数组后，
 * 确定实际的 _alloc_start地址。
 * 
 * 对齐方法：
 * 首先计算出order为0xfff，
 * address加上order，即4KB下的偏移量移到下一页，
 * 然后 & 0x000清空低12位确定地址
 */
static inline reg_t _align_page(reg_t address)
{
    reg_t order = (1 << PAGE_ORDER) - 1;
    return (address + order) & (~order);
}