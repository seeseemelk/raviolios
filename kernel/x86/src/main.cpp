#include "arch.hpp"
#include "defs.hpp"
#include "log.hpp"
#include "multiboot.hpp"
#include "x86.hpp"

multiboot_info_t* Multiboot::mbt;

extern "C" void arch_main(multiboot_info_t* mbt)
{
	Multiboot::mbt = mbt;

	Arch::init_serial();

	size_t moduleCount = mbt->mods_count;
	Log::infof("Found %d modules", moduleCount);

	Arch::panic();
}
