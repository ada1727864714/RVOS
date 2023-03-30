#ifndef __QEMU_VIRT_H__
#define __QEMU_VIRT_H__

/*
 *
 * 设备目前使用QEMU模拟，相关参数
 * QEMU RISC-V Virt machine with 16550a UART and VirtIO MMIO
 * 
 */

/*
 *
 * maximum number of CPUs
 * see https://github.com/qemu/qemu/blob/master/include/hw/riscv/virt.h
 * #define VIRT_CPUS_MAX 8
 * 
 */
#define MAXNUM_CPU 4

/*
 *
 * MemoryMap
 * see https://github.com/qemu/qemu/blob/master/hw/riscv/virt.c, virt_memmap[] 
 * 0x00001000 -- boot ROM, provided by qemu
 * 0x02000000 -- CLINT
 * 0x0C000000 -- PLIC
 * 0x10000000 -- UART0
 * 0x10001000 -- virtio disk
 * 0x80000000 -- boot ROM jumps here in machine mode, where we load our kernel
 * 
 */
/*当前机器将UART寄存器放在物理内存的此处*/
#define UART0 0x10000000UL

#endif