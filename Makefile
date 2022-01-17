ARCH ?= x86
BASE_BIN_DIR = bin
BIN_DIR = $(BASE_BIN_DIR)/$(ARCH)

.PHONY: all clean run
all: kernel
clean:

.PHONY:
test:
	$(MAKE) ARCH=test

include kernel/include.mk
