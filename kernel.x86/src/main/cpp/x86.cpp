#include "x86.hpp"

u8 g_kernel_start;
u8 g_kernel_end;

#define KERNEL_START (reinterpret_cast<u32>(&g_kernel_start) - HIGH_OFFSET)
#define KERNEL_END (reinterpret_cast<u32>(&g_kernel_end) - HIGH_OFFSET - 1)

u32 Arch::kernelStart()
{
	return KERNEL_START;
}

u32 Arch::kernelEnd()
{
	return KERNEL_END;
}

bool Arch::isInPhysicalKernel(u32 addr, size_t length)
{
	u32 end = addr + length - 1;
	return
			  (addr >= KERNEL_START && addr <= KERNEL_END)
			| (end >= KERNEL_START && end <= KERNEL_END)
			| (addr < KERNEL_START && end > KERNEL_END)
			;
	//return !(((addr + length) < KERNEL_START) || (addr > KERNEL_END));
}
