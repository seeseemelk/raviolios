#include "string.h"

#include "defs.hpp"

extern "C" void* memcpy(void* dest, const void* src, size_t count)
{
	u8* destBytes = static_cast<u8*>(dest);
	const u8* srcBytes = static_cast<const u8*>(src);
	for (size_t i = 0; i < count; i++)
		destBytes[i] = srcBytes[i];
	return dest;
}

extern "C" void* memset(void* dest, int ch, size_t count)
{
	u8* destBytes = static_cast<u8*>(dest);
	for (size_t i = 0; i < count; i++)
		destBytes[i] = ch;
	return dest;
}
