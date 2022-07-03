#include "interrupt.hpp"

#include "arch.hpp"
#include "cpu.hpp"
#include "log.hpp"
#include "x86.hpp"

// Will be used later in the source file
#define ISR_NOERRCODE(irq) ISR_ERRCODE(irq)

using namespace Interrupt;

IDT g_idt;
IDTR g_idtr;

extern "C"
{
#define ISR_ERRCODE(irq) void isr_##irq();
#include "interrupt_list.h"
#undef ISR_ERRCODE
}

void Gate::setOffset(void* address)
{
	offset_1 = static_cast<u16>(reinterpret_cast<u32>(address));
	offset_2 = static_cast<u16>(reinterpret_cast<u32>(address) >> 16);
}

size_t Gate::getOffset()
{
	return static_cast<size_t>(offset_1) |
			(static_cast<size_t>(offset_2) << 16);
}

void Gate::setType(GateType type)
{
	zero = 0;
	switch (type)
	{
	case GateType::TASK:
		type_2 = 0b101;
		break;
	case GateType::INTERRUPT:
		type_1 = 0b000;
		type_2 = 0b110;
		break;
	case GateType::TRAP:
		type_1 = 0b000;
		type_2 = 0b111;
		break;
	}
}

Gate& Interrupt::getHandler(size_t irq)
{
	return g_idt.gates[irq];
}

void Interrupt::init()
{
	Log::info("Initialising interrupts");
	cli();

	Log::info("Setting handler targets");
#define ISR_ERRCODE(irq) \
	getHandler(irq).setOffset(reinterpret_cast<void*>(isr_##irq)); \
	getHandler(irq).present = 1; \
	getHandler(irq).segment = 8; \
	getHandler(irq).setType(GateType::INTERRUPT);

#include "interrupt_list.h"

#undef ISR_ERRCODE

	Log::info("preparing IDTR");
	g_idtr.address = reinterpret_cast<u32>(&g_idt);
	g_idtr.limit = sizeof(g_idt) - 1;

	Log::info("Loading IDTR");
	asm("lidt g_idtr");

//	Log::info("Enabling interrupts");
//	sti();
}

extern "C"
{

void g_handle_interrupt_noerror(int irq)
{
	Log::infof("Received interrupt %d", irq);
	Arch::panic();
}

void g_handle_interrupt_error(int irq, int error)
{
	switch (irq)
	{
	case 14: // Page Fault
		Log::infof("Received interrupt %d with error %x at address %x", irq, error, getCR2());
		Arch::dumpMemoryMap();
		break;
	default:
		Log::infof("Received interrupt %d with error %x", irq, error);
		break;
	}
	Arch::panic();
}

}
