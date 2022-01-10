#include "gc.hpp"

using namespace GC;

void* Meta::getRaw()
{
	return static_cast<void*>(this + 1);
}
