/**@file
 * @brief
 */
#ifndef _MEMORY_HPP_
#define _MEMORY_HPP_

#include "defs.hpp"

namespace Memory
{
	/**
	 * A pointer to the heap.
	 */
	extern u8* g_heap;

	/**
	 * The number of bytes available to the heap.
	 */
	extern size_t g_heapSize;

	/**
	 * Initialise memory.
	 */
	void init();

	/**
	 * Appends memory to the space allocated to the heap.
	 *
	 * @param memory The memory region to use for the heap.
	 * @param length The number of bytes that are in the heap.
	 */
//	void appendToHeap(u8* memory, size_t length);
}

#endif /* _MEMORY_HPP_ */
