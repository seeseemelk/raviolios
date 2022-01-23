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
}

#endif
