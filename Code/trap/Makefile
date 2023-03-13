include ../common.mk
INCLUDE_DIR := ../include ../
DIR := -I$(INCLUDE_DIR)

SRCS_C = $(wildcard *.c)

OBJ_C = $(patsubst %.c,%.o,$(SRCS_C)) 

all:$(OBJ_S) $(OBJ_C)

$(OBJ_C):%.o:%.c
	${CC} ${CFLAGS} -c $^ $(DIR) -o $(OBJ_DIR)/$@