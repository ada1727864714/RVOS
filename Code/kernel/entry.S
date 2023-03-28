# 保存所有通用寄存器（General-Purpose(GP)）到上下文中
# 等价于代码：
# struct context *base = &ctx_task;
# base->ra = ra;
# ......
.macro reg_save base
    sd ra, 0(\base)
	sd sp, 8(\base)
	sd gp, 16(\base)
	sd tp, 24(\base)
	sd t0, 32(\base)
	sd t1, 40(\base)
	sd t2, 48(\base)
	sd s0, 56(\base)
	sd s1, 64(\base)
	sd a0, 72(\base)
	sd a1, 80(\base)
	sd a2, 88(\base)
	sd a3, 96(\base)
	sd a4, 104(\base)
	sd a5, 112(\base)
	sd a6, 120(\base)
	sd a7, 128(\base)
	sd s2, 136(\base)
	sd s3, 144(\base)
	sd s4, 152(\base)
	sd s5, 160(\base)
	sd s6, 168(\base)
	sd s7, 176(\base)
	sd s8, 184(\base)
	sd s9, 192(\base)
	sd s10, 200(\base)
	sd s11, 208(\base)
	sd t3, 216(\base)
	sd t4, 224(\base)
	sd t5, 232(\base)
    # 我们这里不需要保存t6，因为我们需要他作为base寄存器
    # 我们需要使用额外的操作在reg_save外保存t6
.endm

# 从上下文中加载所有的通用寄存器
# 等价于代码：
# struct context *base = &ctx_task;
# ra = base->ra;
# ......
.macro reg_restore base
	ld ra, 0(\base)
	ld sp, 8(\base)
	ld gp, 16(\base)
	ld tp, 24(\base)
	ld t0, 32(\base)
	ld t1, 40(\base)
	ld t2, 48(\base)
	ld s0, 56(\base)
	ld s1, 64(\base)
	ld a0, 72(\base)
	ld a1, 80(\base)
	ld a2, 88(\base)
	ld a3, 96(\base)
	ld a4, 104(\base)
	ld a5, 112(\base)
	ld a6, 120(\base)
	ld a7, 128(\base)
	ld s2, 136(\base)
	ld s3, 144(\base)
	ld s4, 152(\base)
	ld s5, 160(\base)
	ld s6, 168(\base)
	ld s7, 176(\base)
	ld s8, 184(\base)
	ld s9, 192(\base)
	ld s10, 200(\base)
	ld s11, 208(\base)
	ld t3, 216(\base)
	ld t4, 224(\base)
	ld t5, 232(\base)
	ld t6, 240(\base)
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
.align 8
trap_vector:
	# 保存当前上下文
	csrrw   t6,mscratch,t6    # 交换t6和mscratch的值
    reg_save t6             # 保存前一任务的上下文

    # 保存实际的t6寄存器
    mv  t5,t6        # t5指向当前任务的上下文
    csrr    t6,mscratch     # 获取t6的实际值
    sd  t6, 240(t5)     # 将t6的值保存在正确的位置（我们定义的是上下文偏移120）

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
.align 8
switch_to:
    csrrw   t6,mscratch,t6    # 交换t6和mscratch的值
    beqz    t6,1f           # 注意：第一次调用switch_to()函数时，
                            # mscratch寄存器被初始化为0（在sched_init()中），
                            # 这使得t6为0，此时直接跳转对mscratch进行初始化
    
    reg_save t6             # 保存前一任务的上下文

    # 保存实际的t6寄存器
    mv  t5,t6        # t5指向当前任务的上下文
    csrr    t6,mscratch     # 获取t6的实际值
    sd  t6, 240(t5)     # 将t6的值保存在正确的位置（我们定义的是上下文偏移120）

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