PREFIX = i386-elf-

KERNEL_ELF = $(KERNEL_BIN_DIR)/raviolios-i386.elf

KERNEL_SRC += \
	$(wildcard kernel/x86/src/libc/*.cpp) \
	$(wildcard kernel/x86/src/*.cpp) \
	$(wildcard kernel/x86/src/*.S)
KERNEL_INC += kernel/x86/inc
KERNEL_LDSCRIPT = kernel/x86/src/kernel.ld

KERNEL_OBJ = $(KERNEL_SRC:%=$(KERNEL_BIN_DIR)/%.o) $(CRTBEGIN_OBJ) $(CRTEND_OBJ)

KERNEL_CFLAGS = --target=i386-elf -march=i486 -Wall -Wextra -Werror -ffreestanding $(KERNEL_INC:%=-I%) -ggdb -fno-builtin -fno-c++-static-destructors
KERNEL_CPPFLAGS = $(KERNEL_CFLAGS) -fno-rtti -fno-exceptions --std=c++17
KERNEL_LDFLAGS = $(KERNEL_CFLAGS) -nostdlib -ffreestanding -lgcc
KERNEL_ASFLAGS = $(KERNEL_CFLAGS)

.PHONY: kernel
kernel: $(KERNEL_ELF)

$(KERNEL_ELF): $(KERNEL_OBJ) $(KERNEL_LDSCRIPT)
	mkdir -p $(dir $@)
	$(CC) $(KERNEL_LDFLAGS) -T $(KERNEL_LDSCRIPT) -o $@.tmp $(KERNEL_OBJ)
	grub-file --is-x86-multiboot $@.tmp 
	mv $@.tmp $@

$(KERNEL_BIN_DIR)/%.cpp.o: %.cpp
	mkdir -p $(dir $@)
	$(CPP) $(KERNEL_CPPFLAGS) -MP -MD -MF $@.d -c -o $@ $<

$(KERNEL_BIN_DIR)/%.S.o: %.S
	mkdir -p $(dir $@)
	$(AS) $(KERNEL_ASFLAGS) -c -o $@ $<
	
.PHONY: clean_kernel
clean: clean_kernel
clean_kernel:
	rm -f $(KERNEL_OBJ) $(KERNEL_ELF)

run: $(KERNEL_ELF)
	qemu-system-i386 -m 16M -gdb tcp::1234 -serial stdio -kernel $(KERNEL_ELF)
	
run-debug: $(KERNEL_ELF)
	qemu-system-i386 -m 16M -gdb tcp::1234 -serial stdio -kernel $(KERNEL_ELF) -S

-include $(KERNEL_OBJ:%=%.d)
