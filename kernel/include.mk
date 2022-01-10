KERNEL_BIN_DIR = $(BIN_DIR)

KERNEL_SRC =
KERNEL_INC =

CPP = $(PREFIX)g++
CC = $(PREFIX)gcc
AS = $(PREFIX)as

include kernel/core/include.mk
include kernel/$(ARCH)/include.mk
