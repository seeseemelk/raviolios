KERNEL_BIN_DIR = $(BIN_DIR)

KERNEL_SRC =
KERNEL_INC =

CPP = clang++
CC = clang
AS = clang

include kernel/core/include.mk
include kernel/$(ARCH)/include.mk
