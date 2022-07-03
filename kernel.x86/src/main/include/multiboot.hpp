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

	struct MapIterator
	{
		multiboot_memory_map_t* entry = nullptr;
		multiboot_memory_map_t* actual = nullptr;
		size_t bytesLeft = 0;

		/**
		 * Gets the next entry in the list.
		 *
		 * @return `true` if the next entry was found, `false` if the end of the
		 * list was reached.
		 */
		bool next();

		/**
		 * Gets the next available entry.
		 *
		 * @return `true` if the next entry was found, `false` if the end of the
		 * list was reached.
		 */
		bool nextAvailable();
	};

}

#endif
