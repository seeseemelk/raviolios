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
Define the offset between the address the kernel is loaded at and the kernel
will run from.
*/
.set VMEM_OFFSET, 0xFFC00000
.set PAGE_PRESENT, 0x00000001
.set PAGE_RW, 0x00000002
.set PAGE_OPTIONS, (PAGE_PRESENT | PAGE_RW)

/*
Define some globals we'll need later.
*/
.set CR0_PG, 1<<31
//.global g_kernel_start
//g_kernel_start:
//.global g_kernel_end
//g_kernel_end:

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
	cli
	/*
	Setup the stack.
	*/
	mov $(stack_top - VMEM_OFFSET), %esp
	mov $(stack_top - VMEM_OFFSET), %ebp

	/*
	Push the arguments we want to pass to main. We'll clobber some registers
	later so we should do this early.
	*/
	push %ebx

	/*
	The page table will identity map to the first 4 MiB of memory. We'll have to
	enable this map for 0x0000_0000 - 0x003F_FFFF and 0xFFC0_0000 - 0xFFFF_FFFF.
	*/
	// Store the value of the page directory entry in eax.
	movl $PAGE_OPTIONS, %eax
	or $(g_page_table - VMEM_OFFSET), %eax

	// Set the first and last page directory entries.
	movl %eax, (g_page_directory - VMEM_OFFSET)
	movl %eax, (g_page_directory - VMEM_OFFSET + 4*1023)

	/*
	Create an identity map. The first entry maps to address 0, the next to address
	4 KiB, the next 8 KiB, and so on...
	*/
	// Need to create 1024 entries
	movl $1024, %ecx
	// eax will contain the physical address that's being mapped
	xor %eax, %eax

	// ebx contains the entry being modified
	movl $(g_page_table - VMEM_OFFSET), %ebx
	.storeTableEntry:
		// Store the options
		movl $PAGE_OPTIONS, (%ebx)
		// Store the physical address
		or %eax, (%ebx)
		// Go to the next address
		addl $4, %ebx
		// Go to the next 4 KiB of phys memory.
		addl $4096, %eax
	// Loop to the next entry
	loop .storeTableEntry

	/*
	Set CR3 to point to the page directory.
	*/
	mov $(g_page_directory - VMEM_OFFSET), %eax
	mov %eax, %cr3

	/*
	Enable paging.
	*/
	mov %cr0, %eax
	or $CR0_PG, %eax
	mov %eax, %cr0

	/*
	Fix ESP and EBP as they still point to the addresses they reside at before
	paging was enabled.
	*/
	add $VMEM_OFFSET, %esp
	add $VMEM_OFFSET, %ebp

	/*
	Call the kernel_main. We _MUST_ have a 16-byte aligned stack at this point,
	otherwise we might have trouble.
	However, there is no reason for the stack not to be aligned as we have just
	configured it.
	We have also pushed ebx as it contains a pointer to the multiboot header.
	Pushing it allows us to access it as a function argument.
	*/
	call arch_main + VMEM_OFFSET

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

/*
The page directory. Initially configured by the assembly code.
*/
.section .bss
.global g_page_directory
.align 4096
g_page_directory:
.skip 4096


/**
The page table containing references to the kernel itself.
*/
.global g_page_table
.align 4096
g_page_table:
.skip 4096
