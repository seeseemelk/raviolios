#ifndef MULTIBOOT_HPP_
#define MULTIBOOT_HPP_

#include "defs.hpp"

extern "C"
{
#include "multiboot.h"
}

namespace Multiboot
{
	extern multiboot_info_t* mbt;

	struct Module
	{
		Module(multiboot_module_t* mmod);

		u8* data;
		size_t length;
	};

	bool hasMultibootFlag(u32 flag);
}

#endif
