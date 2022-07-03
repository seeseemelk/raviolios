/*
 * interrupt.hpp
 *
 *  Created on: Jul 3, 2022
 *      Author: seeseemelk
 */

#ifndef _INTERRUPT_HPP_
#define _INTERRUPT_HPP_

#include "defs.hpp"

/* INTERRUPT LIST
 *  0 00	#DE	Divide Error
 *  1 01	#DB	Debug Exception
 *  2 02		NMI Interrupt
 *  3 03	#BP	Break Point
 *  4 04	#OF	Overflow
 *  5 05	#BR	BOUND Range Exceeded
 *  6 06	#UD	Invalid Opcode
 *  7 07	#NM	Device Not Available (No Math Coprocessor)
 *  8 08	#DF	Double Fault (ERROR CODE)
 *  9 09		Coprocessor Segment Overrun (reserved)
 * 10 0A	#TS	Invalid TSS (ERROR CODE)
 * 11 0B	#NP	Segment not present (ERROR CODE)
 * 12 0C	#SS	Stack-Segment Fault (ERROR CODE)
 * 13 0D	#GP	General Protection (ERROR CODE)
 * 14 0E	#PF	Page Fault (ERROR CODE)
 * 15 0F		(reserved)
 * 16 10	#MF	Math Fault
 * 17 11	#AC	Alignment Check (ERROR CODE)
 * 18 12	#MC	Machine Check
 * 19 13	#XM	SIMD Floating-Point Exception
 * 20 14	#VE	Virtualization Exception
 * 21-32 15-1F	(reserved)
 */

/*
 * Task Gate:
 *
 * Interrupt Gate:
 * Disables the IF flag so that no further exceptions can be fired.
 *
 * Trap Gate:
 * The same as the Interrupt Gate, but it does not disable the IF flag so further exceptions can be fired.
 */

namespace Interrupt
{

	enum GateType
	{
		TASK,
		INTERRUPT,
		TRAP
	};

	struct Gate
	{
		// Byte 1
		u16 offset_1;
		// Byte 2
		u16 segment;

		// Byte 3
		u16 _1 : 5;
		u16 type_1 : 3;
		u16 type_2 : 3;
		u16 size : 1;
		u16 zero : 1;
		u16 dpl : 2;
		u16 present : 1;

		// Byte 4
		u16 offset_2;

		// Members
		void setOffset(void* address);
		size_t getOffset();
		void setType(GateType type);
	} __attribute__((packed));

	struct IDT
	{
		Gate gates[256];
	} __attribute__((aligned(8),packed));

	struct IDTR
	{
		u16 limit;
		u32 address;
	} __attribute__((packed));

	Gate& getHandler(size_t irq);

	void init();
	void disable();
	void enable();
	void handle_interrupt();
}

extern Interrupt::IDT g_idt;
extern Interrupt::IDTR g_idtr;

#endif /* KERNEL_X86_SRC_MAIN_INCLUDE_INTERRUPT_HPP_ */
