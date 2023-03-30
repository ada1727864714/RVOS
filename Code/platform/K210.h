#ifndef __K210_H__
#define __k210_H__

/*
 * 定义K210相关设备参数，参考opensbi实现
 */

#include "../include/types.h"

extern int sifive_uart_init(unsigned long base, uint32_t in_freq, uint32_t baudrate);
extern uint32_t k210_get_clk_freq();
extern uint32_t get_reg(uint32_t num);
extern void set_reg(uint32_t num, uint32_t val);

#define K210_HART_COUNT		2

#define K210_UART_BAUDRATE	115200
#define K210_ACLINT_MTIMER_FREQ	7800000
#define K210_CLK0_FREQ		26000000UL
#define K210_PLIC_NUM_SOURCES	65

/* Registers base address */
#define K210_SYSCTL_BASE_ADDR	0x50440000ULL
#define K210_UART_BASE_ADDR	0x38000000ULL
#define K210_CLINT_BASE_ADDR	0x02000000ULL
#define K210_ACLINT_MSWI_ADDR	\
		(K210_CLINT_BASE_ADDR + CLINT_MSWI_OFFSET)
#define K210_ACLINT_MTIMER_ADDR \
		(K210_CLINT_BASE_ADDR + CLINT_MTIMER_OFFSET)
#define K210_PLIC_BASE_ADDR	0x0C000000ULL

/* Registers */
#define K210_PLL0		0x08
#define K210_CLKSEL0		0x20
#define K210_RESET		0x30

/* Register bit masks */
#define K210_RESET_MASK		0x01



#define UART_REG_TXFIFO		0
#define UART_REG_RXFIFO		1
#define UART_REG_TXCTRL		2
#define UART_REG_RXCTRL		3
#define UART_REG_IE		4
#define UART_REG_IP		5
#define UART_REG_DIV		6

#define UART_TXFIFO_FULL	0x80000000
#define UART_RXFIFO_EMPTY	0x80000000
#define UART_RXFIFO_DATA	0x000000ff
#define UART_TXCTRL_TXEN	0x1
#define UART_RXCTRL_RXEN	0x1

#define UART_REG(reg) ((volatile uint8_t *)(UART0 + reg))

#define __io_br()	do {} while (0)
#define __io_ar()	__asm__ __volatile__ ("fence i,r" : : : "memory");
static inline uint32_t __raw_readl(const volatile void *addr)
{
	uint32_t val;

	asm volatile("lw %0, 0(%1)" : "=r"(val) : "r"(addr));
	return val;
}
#define readl(c)	({ uint32_t __v; __io_br(); __v = __raw_readl(c); __io_ar(); __v; })

#define __io_bw()	__asm__ __volatile__ ("fence w,o" : : : "memory");
#define __io_aw()	do {} while (0)
static inline void __raw_writel(uint32_t val, volatile void *addr)
{
	asm volatile("sw %0, 0(%1)" : : "r"(val), "r"(addr));
}
#define writel(v,c)	({ __io_bw(); __raw_writel((v),(c)); __io_aw(); })


static inline uint32_t k210_read_sysreg(uint32_t reg)
{
	return readl((volatile void *)(K210_SYSCTL_BASE_ADDR + reg));
}

static inline void k210_write_sysreg(uint32_t val, uint32_t reg)
{
	writel(val, (volatile void *)(K210_SYSCTL_BASE_ADDR + reg));
}


#endif