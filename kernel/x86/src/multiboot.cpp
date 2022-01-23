#include "multiboot.hpp"

using namespace Multiboot;

multiboot_info_t* Multiboot::mbt;

Module::Module(multiboot_module_t* mmod)
	: data(reinterpret_cast<u8*>(mmod->mod_start))
	, length(mmod->mod_end - mmod->mod_start)
{
}

bool Multiboot::hasMultibootFlag(u32 flag)
{
	return (Multiboot::mbt->flags & flag) != 0;
}
