platform = K210
CROSS_COMPILE = riscv64-unknown-elf-
CFLAGS = -nostdlib -mcmodel=medany -fno-builtin -march=rv64ima -mabi=lp64 -g -Wall -D$(platform)

QEMU = qemu-system-riscv64
QFLAGS = -nographic -smp 1 -machine virt -bios none

GDB = gdb-multiarch
CC = ${CROSS_COMPILE}gcc
OBJCOPY = ${CROSS_COMPILE}objcopy
OBJDUMP = ${CROSS_COMPILE}objdump

TOP_DIR := $(PWD)

OBJ_DIR := $(TOP_DIR)/obj

BIN_DIR := $(TOP_DIR)/bin

OUT_PUT := $(TOP_DIR)/bin