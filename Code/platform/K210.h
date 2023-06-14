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
extern void uarths_init();
extern int uarths_putchar(char c);
extern int uarths_getchar(void);

#define EOF (-1)

#define UARTHS_BASE_ADDR    (0x38000000U)

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

/*
 * Hi-speed UART interrupt source
 */
#define UARTHS_IRQ 33

/*
 * This machine puts platform-level interrupt controller (PLIC) here.
 * 定义平台级中断控制器（PLIC）的内存映射图与相关寄存器操作。
 * 这里仅列出M模式下的PLIC寄存器。
 */
#ifdef K210
#define PLIC_BASE	0x0C000000ULL
#endif

/************************************************************/
/* plic */

/**
 * @brief      Interrupt Source Priorities
 *
 *             Each external interrupt source can be assigned a priority by
 *             writing to its 32-bit memory-mapped priority register. The
 *             number and value of supported priority levels can vary by
 *             implementa- tion, with the simplest implementations having all
 *             devices hardwired at priority 1, in which case, interrupts with
 *             the lowest ID have the highest effective priority. The priority
 *             registers are all WARL.
 */
typedef struct _plic_source_priorities
{
    /* 0x0C000000: Reserved, 0x0C000004-0x0C000FFC: 1-1023 priorities */
    uint32_t priority[1024];
} __attribute__((packed, aligned(4))) plic_source_priorities_t;

/**
 * @brief       Interrupt Pending Bits
 *
 *              The current status of the interrupt source pending bits in the
 *              PLIC core can be read from the pending array, organized as 32
 *              words of 32 bits. The pending bit for interrupt ID N is stored
 *              in bit (N mod 32) of word (N/32). Bit 0 of word 0, which
 *              represents the non-existent interrupt source 0, is always
 *              hardwired to zero. The pending bits are read-only. A pending
 *              bit in the PLIC core can be cleared by setting enable bits to
 *              only enable the desired interrupt, then performing a claim. A
 *              pending bit can be set by instructing the associated gateway to
 *              send an interrupt service request.
 */
typedef struct _plic_pending_bits
{
    /* 0x0C001000-0x0C00107C: Bit 0 is zero, Bits 1-1023 is pending bits */
    uint32_t u32[32];
    /* 0x0C001080-0x0C001FFF: Reserved */
    uint8_t resv[0xF80];
} __attribute__((packed, aligned(4))) plic_pending_bits_t;

/**
 * @brief       Target Interrupt Enables
 *
 *              For each interrupt target, each device’s interrupt can be
 *              enabled by setting the corresponding bit in that target’s
 *              enables registers. The enables for a target are accessed as a
 *              contiguous array of 32×32-bit words, packed the same way as the
 *              pending bits. For each target, bit 0 of enable word 0
 *              represents the non-existent interrupt ID 0 and is hardwired to
 *              0. Unused interrupt IDs are also hardwired to zero. The enables
 *              arrays for different targets are packed contiguously in the
 *              address space. Only 32-bit word accesses are supported by the
 *              enables array in RV32 systems. Implementations can trap on
 *              accesses to enables for non-existent targets, but must allow
 *              access to the full enables array for any extant target,
 *              treating all non-existent interrupt source’s enables as
 *              hardwired to zero.
 */
typedef struct _plic_target_enables
{
    /* 0x0C002000-0x0C1F1F80: target 0-15871 enables */
    struct
    {
        uint32_t enable[32 * 2]; /* Offset 0x00-0x7C: Bit 0 is zero, Bits 1-1023 is bits*/
    } target[15872 / 2];

    /* 0x0C1F2000-0x0C1FFFFC: Reserved, size 0xE000 */
    uint8_t resv[0xE000];
} __attribute__((packed, aligned(4))) plic_target_enables_t;

/**
 * @brief       PLIC Targets
 *
 *              Target Priority Thresholds The threshold for a pending
 *              interrupt priority that can interrupt each target can be set in
 *              the target’s threshold register. The threshold is a WARL field,
 *              where different implementations can support different numbers
 *              of thresholds. The simplest implementation has a threshold
 *              hardwired to zero.
 *
 *              Target Claim Each target can perform a claim by reading the
 *              claim/complete register, which returns the ID of the highest
 *              priority pending interrupt or zero if there is no pending
 *              interrupt for the target. A successful claim will also
 *              atomically clear the corresponding pending bit on the interrupt
 *              source. A target can perform a claim at any time, even if the
 *              EIP is not set. The claim operation is not affected by the
 *              setting of the target’s priority threshold register.
 *
 *              Target Completion A target signals it has completed running a
 *              handler by writing the interrupt ID it received from the claim
 *              to the claim/complete register. This is routed to the
 *              corresponding interrupt gateway, which can now send another
 *              interrupt request to the PLIC. The PLIC does not check whether
 *              the completion ID is the same as the last claim ID for that
 *              target. If the completion ID does not match an interrupt source
 *              that is currently enabled for the target, the completion is
 *              silently ignored.
 */
typedef struct _plic_target
{
    /* 0x0C200000-0x0FFFF004: target 0-15871 */
    struct
    {
        uint32_t priority_threshold; /* Offset 0x000 */
        uint32_t claim_complete;     /* Offset 0x004 */
        uint8_t resv[0x1FF8];        /* Offset 0x008, Size 0xFF8 */
    } target[15872 / 2];
} __attribute__((packed, aligned(4))) plic_target_t;

/**
 * @brief       Platform-Level Interrupt Controller
 *
 *              PLIC is Platform-Level Interrupt Controller. The PLIC complies
 *              with the RISC-V Privileged Architecture specification, and can
 *              support a maximum of 1023 external interrupt sources targeting
 *              up to 15,872 core contexts.
 */
typedef struct _plic
{
    /* 0x0C000000-0x0C000FFC */
    plic_source_priorities_t source_priorities;
    /* 0x0C001000-0x0C001FFF */
    const plic_pending_bits_t pending_bits;
    /* 0x0C002000-0x0C1FFFFC */
    plic_target_enables_t target_enables;
    /* 0x0C200000-0x0FFFF004 */
    plic_target_t targets;
} __attribute__((packed, aligned(4))) plic_t;

/****************************************************************/

/* clang-format off */
/* Register address offsets */
#define UARTHS_REG_TXFIFO (0x00)
#define UARTHS_REG_RXFIFO (0x04)
#define UARTHS_REG_TXCTRL (0x08)
#define UARTHS_REG_RXCTRL (0x0c)
#define UARTHS_REG_IE     (0x10)
#define UARTHS_REG_IP     (0x14)
#define UARTHS_REG_DIV    (0x18)

/* TXCTRL register */
#define UARTHS_TXEN       (0x01)
#define UARTHS_TXWM(x)    (((x) & 0xffff) << 16)

/* RXCTRL register */
#define UARTHS_RXEN       (0x01)
#define UARTHS_RXWM(x)    (((x) & 0xffff) << 16)

/* IP register */
#define UARTHS_IP_TXWM    (0x01)
#define UARTHS_IP_RXWM    (0x02)
/* clang-format on */

typedef struct _uarths_txdata
{
    /* Bits [7:0] is data */
    uint32_t data : 8;
    /* Bits [30:8] is 0 */
    uint32_t zero : 23;
    /* Bit 31 is full status */
    uint32_t full : 1;
} __attribute__((packed, aligned(4))) uarths_txdata_t;

typedef struct _uarths_rxdata
{
    /* Bits [7:0] is data */
    uint32_t data : 8;
    /* Bits [30:8] is 0 */
    uint32_t zero : 23;
    /* Bit 31 is empty status */
    uint32_t empty : 1;
} __attribute__((packed, aligned(4))) uarths_rxdata_t;

typedef struct _uarths_txctrl
{
    /* Bit 0 is txen, controls whether the Tx channel is active. */
    uint32_t txen : 1;
    /* Bit 1 is nstop, 0 for one stop bit and 1 for two stop bits */
    uint32_t nstop : 1;
    /* Bits [15:2] is reserved */
    uint32_t resv0 : 14;
    /* Bits [18:16] is threshold of interrupt triggers */
    uint32_t txcnt : 3;
    /* Bits [31:19] is reserved */
    uint32_t resv1 : 13;
} __attribute__((packed, aligned(4))) uarths_txctrl_t;

typedef struct _uarths_rxctrl
{
    /* Bit 0 is txen, controls whether the Tx channel is active. */
    uint32_t rxen : 1;
    /* Bits [15:1] is reserved */
    uint32_t resv0 : 15;
    /* Bits [18:16] is threshold of interrupt triggers */
    uint32_t rxcnt : 3;
    /* Bits [31:19] is reserved */
    uint32_t resv1 : 13;
} __attribute__((packed, aligned(4))) uarths_rxctrl_t;

typedef struct _uarths_ip
{
    /* Bit 0 is txwm, raised less than txcnt */
    uint32_t txwm : 1;
    /* Bit 1 is txwm, raised greater than rxcnt */
    uint32_t rxwm : 1;
    /* Bits [31:2] is 0 */
    uint32_t zero : 30;
} __attribute__((packed, aligned(4))) uarths_ip_t;

typedef struct _uarths_ie
{
    /* Bit 0 is txwm, raised less than txcnt */
    uint32_t txwm : 1;
    /* Bit 1 is txwm, raised greater than rxcnt */
    uint32_t rxwm : 1;
    /* Bits [31:2] is 0 */
    uint32_t zero : 30;
} __attribute__((packed, aligned(4))) uarths_ie_t;

typedef struct _uarths_div
{
    /* Bits [31:2] is baud rate divisor register */
    uint32_t div : 16;
    /* Bits [31:16] is 0 */
    uint32_t zero : 16;
} __attribute__((packed, aligned(4))) uarths_div_t;

typedef struct _uarths
{
    /* Address offset 0x00 */
    uarths_txdata_t txdata;
    /* Address offset 0x04 */
    uarths_rxdata_t rxdata;
    /* Address offset 0x08 */
    uarths_txctrl_t txctrl;
    /* Address offset 0x0c */
    uarths_rxctrl_t rxctrl;
    /* Address offset 0x10 */
    uarths_ie_t ie;
    /* Address offset 0x14 */
    uarths_ip_t ip;
    /* Address offset 0x18 */
    uarths_div_t div;
} __attribute__((packed, aligned(4))) uarths_t;

typedef enum _uarths_interrupt_mode
{
    UARTHS_SEND = 1,
    UARTHS_RECEIVE = 2,
    UARTHS_SEND_RECEIVE = 3,
} uarths_interrupt_mode_t;

typedef enum _uarths_stopbit
{
    UARTHS_STOP_1,
    UARTHS_STOP_2
} uarths_stopbit_t;

#endif