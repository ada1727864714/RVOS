include ../common.mk
INCLUDE_DIR := ../include ../
DIR := -I$(INCLUDE_DIR)

SRCS_C = $(wildcard *.c)
OBJ = $(patsubst %.c,%.o,$(SRCS_C)) 

all:$(OBJ)

$(OBJ):%.o:%.c
	${CC} ${CFLAGS} -c $^ $(DIR) -o $(OBJ_DIR)/$@