
.section .text
.global asm_
.type _start, @function

.global g_handle_interrupt_noerror
.type g_handle_interrupt_noerror, @function

.global g_handle_interrupt_error
.type g_handle_interrupt_error, @function

.global sti
.type sti, @function
sti:
	sti
	ret

.global cli
.type cli, @function
cli:
	cli
	ret

.global reloadPageTable
.type reloadPageTable, @function
reloadPageTable:
	mov %cr3, %eax
	mov %eax, %cr3
	ret

.global getCR2
.type getCR2, @function
getCR2:
	mov %cr2, %eax
	ret

#define ISR_NOERRCODE(irq) \
	.global isr_##irq; \
	.type isr_##irq, @function; \
	isr_##irq:; \
		push $irq; \
		call g_handle_interrupt_noerror; \
		pop %eax; \
		iret;

#define ISR_ERRCODE(irq) \
	.global isr_##irq; \
	.type isr_##irq, @function; \
	isr_##irq:; \
		push $irq; \
		call g_handle_interrupt_error; \
		pop %eax; \
		iret;

#include "interrupt_list.h"
