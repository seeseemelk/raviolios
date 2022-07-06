#include "multiboot.hpp"

#include "x86.hpp"
#include "cpu.hpp"

using namespace Multiboot;

multiboot_info_t* Multiboot::mbt;

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
