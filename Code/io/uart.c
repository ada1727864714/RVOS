#include "../include/os.h"

/* 
 * UART 控制寄存器在UART0地址上进行内存映射
 * 这个会返回你需要的寄存器的地址
 */
#define UART_REG(reg) ((volatile uint8_t *)(UART0 + reg))

/*
 * Reference
 * [1]: TECHNICAL DATA ON 16550, http://byterunner.com/16550.html
 * 16500技术手册
 */

/*
 * UART control registers map. see [1] "PROGRAMMING TABLE"
 * 注意有一些寄存器被复用
 * 0 (write mode): THR/DLL
 * 1 (write mode): IER/DLM
 */
#define RHR 0	// Receive Holding Register (read mode) 接受保存寄存器
#define THR 0	// Transmit Holding Register (write mode) 发送保存寄存器
#define DLL 0	// LSB of Divisor Latch (write mode) 除数锁存器LSB
#define IER 1	// Interrupt Enable Register (write mode) 中断使能寄存器
#define DLM 1	// MSB of Divisor Latch (write mode) 除数锁存器MSB
#define FCR 2	// FIFO Control Register (write mode) FIFO控制寄存器
#define ISR 2	// Interrupt Status Register (read mode) 中断状态寄存器
#define LCR 3	// Line Control Register 线路控制寄存器
#define MCR 4	// Modem Control Register 调制解调器控制寄存器
#define LSR 5	// Line Status Register 线路状态寄存器
#define MSR 6	// Modem Status Register 调制解调器状态寄存器
#define SPR 7	// ScratchPad Register 

/*
 * 上电默认设置
 * IER = 0: TX/RX holding register interrupts are both disabled
 * ISR = 1: no interrupt penting
 * LCR = 0
 * MCR = 0
 * LSR = 60 HEX
 * MSR = BITS 0-3 = 0, BITS 4-7 = inputs
 * FCR = 0
 * TX = High
 * OP1 = High
 * OP2 = High
 * RTS = High
 * DTR = High
 * RXRDY = High
 * TXRDY = Low
 * INT = Low
 */

/*
 * LINE STATUS REGISTER (LSR)线路状态寄存器
 * LSR BIT 0:
 * 0 = no data in receive holding register or FIFO.
 * （RHR或FIFO中没用数据）
 * 1 = data has been receive and saved in the receive holding register or FIFO.
 * （数据已经被接受并保存在RHR或FIFO中）
 * ......
 * LSR BIT 5:
 * 0 = transmit holding register is full. 16550 will not accept any data for transmission.
 * （THR已满，16550将不接受任何数据传输）
 * 1 = transmitter hold register (or FIFO) is empty. CPU can load the next character.
 * （THR或FIFO空闲，CPU可以加载下一个字符）
 * ......
 */
#define LSR_RX_READY (1 << 0)
#define LSR_TX_IDLE  (1 << 5)

#define uart_read_reg(reg) (*(UART_REG(reg)))
#define uart_write_reg(reg, v) (*(UART_REG(reg)) = (v))

void uart_init()
{
#ifdef K210
	uarths_init();
	// sifive_uart_init(K210_UART_BASE_ADDR, k210_get_clk_freq(),
	// 			K210_UART_BAUDRATE);
#else

	/* disable interrupts. */
	uart_write_reg(IER, 0x00);

	/*
	 * 设置波特率。
	 * Setting baud rate. Just a demo here if we care about the divisor,
	 * but for our purpose [QEMU-virt], this doesn't really do anything.
	 *
	 * Notice that the divisor register DLL (divisor latch least) and DLM (divisor
	 * latch most) have the same base address as the receiver/transmitter and the
	 * interrupt enable register. To change what the base address points to, we
	 * open the "divisor latch" by writing 1 into the Divisor Latch Access Bit
	 * (DLAB), which is bit index 7 of the Line Control Register (LCR).
	 * 注意DLL与DLM和RHR,THR,IER有相同的地址，为了选择我们要写的寄存器，要设置LCR寄存器
	 * 第7位为1
	 *
	 * Regarding the baud rate value, see [1] "BAUD RATE GENERATOR PROGRAMMING TABLE".
	 * We use 38.4K when 1.8432 MHZ crystal, so the corresponding value is 3.
	 * And due to the divisor register is two bytes (16 bits), so we need to
	 * split the value of 3(0x0003) into two bytes, DLL stores the low byte,
	 * DLM stores the high byte.
	 * 波特率数值见表，我们使用38.4K时的1.8432 MHZ的晶体，所以对应的值是3。
	 * 由于除数寄存器是两个字节（16位），所以要分开设置，低位DLL设为3，高位DLM设为0
	 */
	uint8_t lcr = uart_read_reg(LCR);
	uart_write_reg(LCR, lcr | (1 << 7));
	uart_write_reg(DLL, 0x03);
	uart_write_reg(DLM, 0x00);

	/*
	 * Continue setting the asynchronous data communication format.
	 * 设置异步数据通信格式。
	 * 设置方式见表
	 * - number of the word length: 8 bits
	 * - number of stop bits：1 bit when word length is 8 bits
	 * - no parity
	 * - no break control
	 * - disabled baud latch
	 */
	lcr = 0;
	uart_write_reg(LCR, lcr | (3 << 0));

	/*
	 * enable receive interrupts.（使能接收中断）
	 */
	uint8_t ier = uart_read_reg(IER);
	uart_write_reg(IER, ier | (1 << 0));
#endif
}

int uart_putc(char ch)
{
#ifdef K210
    uarths_putchar(ch);
	// while (get_reg(UART_REG_TXFIFO) & UART_TXFIFO_FULL)
	// 	;

	// set_reg(UART_REG_TXFIFO, ch);
#else
	
	while ((uart_read_reg(LSR) & LSR_TX_IDLE) == 0);
	return uart_write_reg(THR, ch);
#endif
}

void uart_puts(char *s)
{
	while (*s) {
		uart_putc(*s++);
	}
}

int uart_getc(void)
{
#ifdef K210
	return uarths_getchar();
#else
	if (uart_read_reg(LSR) & LSR_RX_READY){
		return uart_read_reg(RHR);
	} else {
		return -1;
	}
#endif
}

/*
 * handle a uart interrupt, raised because input has arrived, called from trap.c.
 */
void uart_isr(void)
{
	while (1) {
		int c = uart_getc();
		if (c == -1) {
			break;
		} else {
			uart_putc((char)c);
			uart_putc('\n');
		}
	}
}

