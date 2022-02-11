#include "gc.hpp"
#include "log.hpp"

using namespace GC;

void Meta::validate() const
{
	if (guard1 != 0xdeadbeef)
	{
		Log::critical("Broken guard 1");
		Arch::panic();
	}
	if (guard2 != 0x1234abcd)
	{
		Log::critical("Broken guard 2");
		Arch::panic();
	}
}

void* Meta::getRaw()
{
	validate();
	return static_cast<void*>(this + 1);
}

const void* Meta::getRaw() const
{
	validate();
	return static_cast<const void*>(this + 1);
}
