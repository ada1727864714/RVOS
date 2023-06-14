#include "K210.h"

/*
 * 实现K210相关设备函数，参考opensbi实现
 */

volatile uarths_t *const uarths = (volatile uarths_t *)UARTHS_BASE_ADDR;

uint32_t k210_get_clk_freq(void)
{
	uint32_t clksel0, pll0;
	uint64_t pll0_freq, clkr0, clkf0, clkod0, div;

	/*
	 * If the clock selector is not set, use the base frequency.
	 * Otherwise, use PLL0 frequency with a frequency divisor.
	 */
	clksel0 = k210_read_sysreg(K210_CLKSEL0);
	if (!(clksel0 & 0x1))
		return K210_CLK0_FREQ;

	/*
	 * Get PLL0 frequency:
	 * freq = base frequency * clkf0 / (clkr0 * clkod0)
	 */
	pll0 = k210_read_sysreg(K210_PLL0);
	clkr0 = 1 + (pll0 & 0x0000000f);
	clkf0 = 1 + ((pll0 & 0x000003f0) >> 4);
	clkod0 = 1 + ((pll0 & 0x00003c00) >> 10);
	pll0_freq = clkf0 * K210_CLK0_FREQ / (clkr0 * clkod0);

	/* Get the frequency divisor from the clock selector */
	div = 2ULL << ((clksel0 & 0x00000006) >> 1);

	return pll0_freq / div;
}

static volatile char *uart_base;
static uint32_t uart_in_freq;
static uint32_t uart_baudrate;

void set_reg(uint32_t num, uint32_t val)
{
	writel(val, uart_base + (num * 0x4));
}

uint32_t get_reg(uint32_t num)
{
	return readl(uart_base + (num * 0x4));
}

static inline unsigned int uart_min_clk_divisor(uint64_t in_freq,
						uint64_t max_target_hz)
{
	uint64_t quotient = (in_freq + max_target_hz - 1) / (max_target_hz);

	/* Avoid underflow */
	if (quotient == 0)
		return 0;
	else
		return quotient - 1;
}

int sifive_uart_init(unsigned long base, uint32_t in_freq, uint32_t baudrate)
{
	uart_base     = (volatile char *)base;
	uart_in_freq  = in_freq;
	uart_baudrate = baudrate;

	/* Configure baudrate */
	if (in_freq && baudrate)
		set_reg(UART_REG_DIV, uart_min_clk_divisor(in_freq, baudrate));

	// /* Disable interrupts */
	// set_reg(UART_REG_IE, 0);

	set_reg(UART_REG_IP,3);
	/* enable receiving interruption */
	set_reg(UART_REG_IE, 2);

	/* Enable TX */
	set_reg(UART_REG_TXCTRL, UART_TXCTRL_TXEN);

	/* Enable Rx */
	set_reg(UART_REG_RXCTRL, UART_RXCTRL_RXEN);

	//sbi_console_set_device(&sifive_console);

	return 0;
}

void uarths_init(){
	uint32_t freq = k210_get_clk_freq();
	uint16_t div = freq / 115200 - 1;

	/* Set UART registers */
    uarths->div.div = div;
    uarths->txctrl.txen = 1;
    uarths->rxctrl.rxen = 1;
    uarths->txctrl.txcnt = 0;
    uarths->rxctrl.rxcnt = 0;
    uarths->ip.txwm = 1;
    uarths->ip.rxwm = 1;
    uarths->ie.txwm = 0;
    uarths->ie.rxwm = 0;
}

int uarths_putchar(char c)
{
    while(uarths->txdata.full)
        continue;
    uarths->txdata.data = (uint8_t)c;

    return (c & 0xff);
}

int uarths_getchar(void)
{
    /* while not empty */
    uarths_rxdata_t recv = uarths->rxdata;

    if(recv.empty)
        return EOF;
    else
        return (recv.data & 0xff);
}