include ../common.mk
INCLUDE_DIR := ../include ../
DIR := -I$(INCLUDE_DIR)

SRCS_S = $(wildcard *.S)
SRCS_C = $(wildcard *.c)

OBJ_S = $(patsubst %.S,%.o,$(SRCS_S)) 
OBJ_C = $(patsubst %.c,%.o,$(SRCS_C)) 

all:$(OBJ_S) $(OBJ_C)

$(OBJ_S):%.o:%.S
	${CC} ${CFLAGS} -c $^ $(DIR) -o $(OBJ_DIR)/$@

$(OBJ_C):%.o:%.c
	${CC} ${CFLAGS} -c $^ $(DIR) -o $(OBJ_DIR)/$@