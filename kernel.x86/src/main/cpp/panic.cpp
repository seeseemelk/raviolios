#include "arch.hpp"
#include "log.hpp"

void Arch::panic()
{
	Log::critical("Panicked!");
	for (;;)
	{
	}
}
