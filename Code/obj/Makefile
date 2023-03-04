include ../common.mk

OBJS = start.o
OBJS += *.o

# start.o must be the first in dependency!
$(OUT_PUT)/os.elf: ${OBJS}
	${CC} ${CFLAGS} -T os.ld -o $(OUT_PUT)/os.elf $^
	${OBJCOPY} -O binary $(OUT_PUT)/os.elf $(OUT_PUT)/os.bin

