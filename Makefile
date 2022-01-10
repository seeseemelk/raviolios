ARCH ?= x86
BIN_DIR = bin/$(ARCH)

.PHONY: all clean run
all: kernel
clean:

.PHONY:
test:
	$(MAKE) ARCH=test

include kernel/include.mk
