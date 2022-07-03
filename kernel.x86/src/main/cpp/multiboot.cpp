#include "multiboot.hpp"

#include "x86.hpp"
#include "cpu.hpp"

using namespace Multiboot;

multiboot_info_t* Multiboot::mbt;
static multiboot_memory_map_t s_splitEntry;

#define MULTIBOOT_MEMORY_SPLIT_LOW (MULTIBOOT_MEMORY_BADRAM+1)
#define MULTIBOOT_MEMORY_SPLIT_HIGH (MULTIBOOT_MEMORY_SPLIT_LOW+1)

///**
// * Patch the memory map given by the bootloader so it does not mark
// * the kernel's memory as 'free'.
// */
//static void patchMemoryMap()
//{
//	Multiboot::MapIterator iterator;
//	while (iterator.nextAvailable())
//	{
//		u64 addr = iterator.entry->addr;
//		u64 length = iterator.entry->len;
//		if (Arch::isInPhysicalKernel(addr, length))
//		{
//			s_splitEntry.
//		}
//	}
//}

Module::Module(multiboot_module_t* mmod)
	: data(Arch::offsetToHigh<u8>(mmod->mod_start))
	, length(mmod->mod_end - mmod->mod_start)
{
}

bool Multiboot::hasMultibootFlag(u32 flag)
{
	return (Multiboot::mbt->flags & flag) != 0;
}

bool MapIterator::next()
{
	if (entry == nullptr)
	{
		entry = reinterpret_cast<multiboot_memory_map_t*>(mbt->mmap_addr);
		bytesLeft = mbt->mmap_length;
		return true;
	}
	else if (entry->type == MULTIBOOT_MEMORY_SPLIT_LOW)
	{
		entry->addr = Arch::kernelStart();
		entry->len = (actual->addr + actual->len) - Arch::kernelEnd();
		entry->type = MULTIBOOT_MEMORY_SPLIT_HIGH;
		return true;
	}
	else if (entry->type == MULTIBOOT_MEMORY_SPLIT_HIGH)
	{
		u8* mem = reinterpret_cast<u8*>(actual);
		entry = reinterpret_cast<multiboot_memory_map_t*>(mem + actual->size + 4);
		bytesLeft -= entry->size + 4;
		return true;
	}
	else if (Arch::isInPhysicalKernel(entry->addr, entry->len))
	{
		actual = entry;
		entry = &s_splitEntry;
		entry->addr = actual->addr;
		entry->len = Arch::kernelStart() - entry->addr;
		entry->type = MULTIBOOT_MEMORY_SPLIT_LOW;
		return true;
	}
	else if (bytesLeft > 0)
	{
		u8* mem = reinterpret_cast<u8*>(entry);
		entry = reinterpret_cast<multiboot_memory_map_t*>(mem + entry->size + 4);
		bytesLeft -= entry->size + 4;
		return true;
	}
	else
	{
		return false;
	}
}

bool MapIterator::nextAvailable()
{
	while (next())
	{
		if (entry->type == MULTIBOOT_MEMORY_AVAILABLE)
			return true;
	}
	return false;
}
