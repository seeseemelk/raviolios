#include "arch.hpp"

extern "C" void __cxa_pure_virtual()
{
	Arch::panic();
}

//void* __dso_handle;
