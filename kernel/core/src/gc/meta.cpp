#include "gc.hpp"

using namespace GC;

void* Meta::getRaw()
{
	return static_cast<void*>(this + 1);
}

const void* Meta::getRaw() const
{
	return static_cast<const void*>(this + 1);
}
