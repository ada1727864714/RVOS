# 保存所有通用寄存器（General-Purpose(GP)）到上下文中
# 等价于代码：
# struct context *base = &ctx_task;
# base->ra = ra;
# ......
.macro reg_save base
    sw ra, 0(\base)
    sw sp, 4(\base)
	sw gp, 8(\base)
	sw tp, 12(\base)
	sw t0, 16(\base)
	sw t1, 20(\base)
	sw t2, 24(\base)
	sw s0, 28(\base)
	sw s1, 32(\base)
	sw a0, 36(\base)
	sw a1, 40(\base)
	sw a2, 44(\base)
	sw a3, 48(\base)
	sw a4, 52(\base)
	sw a5, 56(\base)
	sw a6, 60(\base)
	sw a7, 64(\base)
	sw s2, 68(\base)
	sw s3, 72(\base)
	sw s4, 76(\base)
	sw s5, 80(\base)
	sw s6, 84(\base)
	sw s7, 88(\base)
	sw s8, 92(\base)
	sw s9, 96(\base)
	sw s10, 100(\base)
	sw s11, 104(\base)
	sw t3, 108(\base)
	sw t4, 112(\base)
	sw t5, 116(\base)
    # 我们这里不需要保存t6，因为我们需要他作为base寄存器
    # 我们需要使用额外的操作在reg_save外保存t6
.endm

# 从上下文中加载所有的通用寄存器
# 等价于代码：
# struct context *base = &ctx_task;
# ra = base->ra;
# ......
.macro reg_restore base
	lw ra, 0(\base)
	lw sp, 4(\base)
	lw gp, 8(\base)
	lw tp, 12(\base)
	lw t0, 16(\base)
	lw t1, 20(\base)
	lw t2, 24(\base)
	lw s0, 28(\base)
	lw s1, 32(\base)
	lw a0, 36(\base)
	lw a1, 40(\base)
	lw a2, 44(\base)
	lw a3, 48(\base)
	lw a4, 52(\base)
	lw a5, 56(\base)
	lw a6, 60(\base)
	lw a7, 64(\base)
	lw s2, 68(\base)
	lw s3, 72(\base)
	lw s4, 76(\base)
	lw s5, 80(\base)
	lw s6, 84(\base)
	lw s7, 88(\base)
	lw s8, 92(\base)
	lw s9, 96(\base)
	lw s10, 100(\base)
	lw s11, 104(\base)
	lw t3, 108(\base)
	lw t4, 112(\base)
	lw t5, 116(\base)
	lw t6, 120(\base)
.endm

# 关于保存/恢复需要注意的是：
# 1.我们使用mscratch寄存器来保存一个指向当前任务上下文的指针
# 2.我们使用t6作为base来进行reg_save/reg_restore，因为t6是
#   最底部的寄存器（x31），在加载过程中不会被覆盖。
# 注意：CSRs（mscratch）不能用作'base'，因为加载/恢复指令只接受通用寄存器

.text

# 机器模式下的中断和异常出现在这里
.globl trap_vector
# trap向量基址必须始终在4字节边界上对齐
.align 4
trap_vector:
	# 保存当前上下文
	csrrw   t6,mscratch,t6    # 交换t6和mscratch的值
    reg_save t6             # 保存前一任务的上下文

    # 保存实际的t6寄存器
    mv  t5,t6        # t5指向当前任务的上下文
    csrr    t6,mscratch     # 获取t6的实际值
    sw  t6, 120(t5)     # 将t6的值保存在正确的位置（我们定义的是上下文偏移120）

	# 将上下文指针恢复到 mscratch
	csrw	mscratch, t5

	# 调用在trap.c中的trap处理函数
	csrr	a0, mepc
	csrr	a1,	mcause
	call	trap_handler

	# trap_handler会通过a0返回返回地址
	csrw	mepc, a0

	# 恢复上下文
	csrr	t6, mscratch
	reg_restore t6

	# 回到trap之前做的事情
	mret

# void switch_to(struct context *next);
# a0：指向下一个任务的上下文的指针
.globl switch_to
.align 4
switch_to:
    csrrw   t6,mscratch,t6    # 交换t6和mscratch的值
    beqz    t6,1f           # 注意：第一次调用switch_to()函数时，
                            # mscratch寄存器被初始化为0（在sched_init()中），
                            # 这使得t6为0，此时直接跳转对mscratch进行初始化
    
    reg_save t6             # 保存前一任务的上下文

    # 保存实际的t6寄存器
    mv  t5,t6        # t5指向当前任务的上下文
    csrr    t6,mscratch     # 获取t6的实际值
    sw  t6, 120(t5)     # 将t6的值保存在正确的位置（我们定义的是上下文偏移120）

1:
    # 设置mscratch寄存器的值指向新任务的上下文
    csrw    mscratch, a0

    # 加载所有的通用寄存器
    # 此时我们需要使用t6存放上下文地址，
    # 因为如果使用a0那么在加载过程中就会被覆盖掉
    # 而t6是在最后加载
    mv  t6, a0
    reg_restore t6


    ret

.end