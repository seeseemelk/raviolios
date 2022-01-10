#include "arch.hpp"
#include "log.hpp"
#include "x86.hpp"

extern "C" void arch_main()
{
	Arch::init_serial();

	Log::info("Hello, world!");

	Arch::panic();
}
