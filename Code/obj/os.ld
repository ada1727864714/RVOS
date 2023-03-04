/*
 * rvos.ld
 * Linker script for outputing to RVOS
 */

/*
 * https://sourceware.org/binutils/docs/ld/Miscellaneous-Commands.html
 * OUTPUT_ARCH命令指定特定的输出机器体系结构
 * "riscv" is the name of the architecture for both 64-bit and 32-bit
 * We will further refine this by using -march=rv32ima
 * and -mabi=ilp32 when calling gcc.
 */
OUTPUT_ARCH("riscv")

/*
 * https://sourceware.org/binutils/docs/ld/Entry-Point.html
 * ENTRY command is used to set the "entry point", which is the first instruction
 * to execute in a program.
 * The argument of ENTRY command is a symbol name, here is "_start" which is
 * defined in start.S.
 */
ENTRY(_start)

/*
 * https://sourceware.org/binutils/docs/ld/MEMORY.html
 * MEMORY命令描述内存在target中的位置和大小
 * MEMORY的格式如下:
 * MEMORY
 * {
 *     name [(attr)] : ORIGIN = origin, LENGTH = len
 *     ......
 * }
 * 每一行都定义了一块内存区域
 * 每一块内存区域都必须有确切的名字与 MEMORY 命令。这里我们指定义一块名为 "ram" 的区域
 * 字符串是一个可选的属性列表，用于指定是否为未在链接器脚本中显式映射的输入节使用特定的内存区域。
 * 如SECTIONS命令中所述，如果没有为某个输入节指定输出节，链接器将创建一个与输入节同名的输出节。
 * 如果定义区域属性，链接器将使用它们为其创建的输出节选择内存区域。
 * 各参数以及定义如下：
 * 'r' :Read-only section
 * 'w' :Read/write section
 * 'x' :Executable section
 * 'a' :Allocatable section
 * 'i' :Initialized section
 * 'l' :Same as 'i'
 * '!' :反转以下任何属性的意义
 *
 * 'ORIGIN' 用来设置内存的起始地址，这里我们设置为0x8000_0000，因为这里是QEMU-virt machine
 * 开始执行的地方
 * 最后设置 LENGTH = 128M来告诉链接器RAM有128M byte的大小
 * 链接器会再次检查，以确保所有东西都能装得下。
 */
MEMORY
{
	ram   (wxa!ri) : ORIGIN = 0x80000000, LENGTH = 128M
}

/*
 * https://sourceware.org/binutils/docs/ld/SECTIONS.html
 * SECTIONS命令告诉链接器如何将input sections映射到output sections中，以及如何将
 * output sections放到内存中
 * SECTIONS命令的格式为：
 * SECTIONS
 * {
 *     sections-command
 *     sections-command
 *     ......
 * }
 *
 * 每一个sections命令都可以是以下之一
 * (1) 
 * (1) an ENTRY command
 * (2) a symbol assignment（符号赋值）
 * (3) an output section description（输出节描述）
 * (4) an overlay description（覆盖描述）
 * 在我们的代码中仅使用了 (2) & (3).
 *
 * 我们使用 PROVIDE 命令来定义符号
 * https://sourceware.org/binutils/docs/ld/PROVIDE.html
 * PROVIDE关键字可以用来定义一个符号，格式如下：
 * PROVIDE(symbol = expression)
 * 类似 "_text_start", "_text_end" ... 这样的符号将被用到 mem.S中。
 * 注意：句号'.'告诉链接器设置符号(e.g. _text_start)到当前地址('.' = current memory location)
 * 当我们添加东西时，当前内存位置会移动。
 */
SECTIONS
{
	/*
	 * 我们将在 .text output section中布局所有文本节，从.test开始
     * 括号前的星号 ("*") 表示匹配任何目标文件的.test部分
     * *(.text .text.*)为例，.text匹配.text的段
     * .text.* 使用正则匹配（*匹配全部）诸如 .text.a , .text.b
     * 之类的形式（可以这么写的其实是）
	 */
	.text : {
		PROVIDE(_text_start = .);
		*(.text .text.*)
		PROVIDE(_text_end = .);
	} >ram

	.rodata : {
		PROVIDE(_rodata_start = .);
		*(.rodata .rodata.*)
		PROVIDE(_rodata_end = .);
	} >ram

	.data : {
		/*
		 * . = ALIGN(4096) 告诉链接器将当前内存位置对齐到4096字节。这将
         * 插入填充字节，直到当前位置与4096字节边界对齐为止。因为我们的
         * 分页系统分辨率是4096字节
		 */
		. = ALIGN(4096);
		PROVIDE(_data_start = .);
		/*
		 * sdata和data本质上是一样的，我们不需要区分sdata和data 
		 */
		*(.sdata .sdata.*)
		*(.data .data.*)
		PROVIDE(_data_end = .);
	} >ram

	.bss :{
		/*
		 * https://sourceware.org/binutils/docs/ld/Input-Section-Common.html
		 * 在大多数情况下，输入文件中的常用符号将放在输出文件的 '.bss' 节中 
		 */
		PROVIDE(_bss_start = .);
		*(.sbss .sbss.*)
		*(.bss .bss.*)
		*(COMMON)
		PROVIDE(_bss_end = .);
	} >ram

	PROVIDE(_memory_start = ORIGIN(ram));
	PROVIDE(_memory_end = ORIGIN(ram) + LENGTH(ram));

	PROVIDE(_heap_start = _bss_end);
	PROVIDE(_heap_size = _memory_end - _heap_start);
}
