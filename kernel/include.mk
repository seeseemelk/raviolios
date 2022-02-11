KERNEL_BIN_DIR = $(BIN_DIR)

KERNEL_SRC =
KERNEL_INC =

#CPP = clang++
#CC = clang
#AS = clang
CPP = g++
CC = gcc
AS = as

include kernel/core/include.mk
include kernel/$(ARCH)/include.mk
