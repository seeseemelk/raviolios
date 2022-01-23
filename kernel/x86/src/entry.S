/*
This multiboot compliant header was based instructions from
http://wiki.osdev.org/Bare_Bones
*/

.set ALIGN, 1<<0 /* Align loaded modules on page boundaries */
.set MEMINFO, 1<<1 /* Provide memory map */
.set FLAGS, ALIGN | MEMINFO /* This is the Multiboot 'flag' field */
.set MAGIC, 0x1BADB002 /* 'magic number' lets bootloader find the header */
.set CHECKSUM, -(MAGIC + FLAGS) /* Checksum of above, to prove we are multiboot */

/*
Declare the Multiboot header
*/
.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

/*
Multiboot does not setup the stack for you. We have to do that ourselves.
*/
.section .bss
.align 16
stack_bottom:
.skip 16384
stack_top:

/*
The entry point to the kernel.
*/
.section .text
.global _start
.type _start, @function
_start:
	/*
	Setup the stack.
	*/
	mov $stack_top, %esp

	/*
	Call the kernel_main. We _MUST_ have a 16-byte aligned stack at this point,
	otherwise we might have trouble.
	However, there is no reason for the stack not to be aligned as we have just
	configured it.
	*/
	push %ebx
	call arch_main

	/*
	Loop if we ever return from kernel_main.
	*/
	cli
.hang:
	hlt
	jmp .hang

/*
Set the size of the _start symbol.
*/
.size _start, . - _start
