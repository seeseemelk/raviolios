/**@file
 * @brief
 */
#ifndef _X86_HPP_
#define _X86_HPP_

#include "defs.hpp"

/// The offset from low memoryto high memory.
#define HIGH_OFFSET 0xFFC00000

namespace Arch
{
	u32 kernelStart();
	u32 kernelEnd();
	void init_serial();
	void dumpMemoryMap();

	/**
	 * Checks if an address is in the kernel's space.
	 */
	bool isInPhysicalKernel(u32 addr, size_t length);

	template<typename T>
	inline T* offsetToHigh(T* ptr)
	{
		return reinterpret_cast<T*>(reinterpret_cast<u8*>(ptr) + HIGH_OFFSET);
	}

	template<typename T>
	inline T* offsetToHigh(u32 ptr)
	{
		return offsetToHigh(reinterpret_cast<T*>(ptr));
	}
}

#endif /* _X86_HPP_ */
