#include "arch.hpp"
#include "defs.hpp"
#include "log.hpp"
#include "multiboot.hpp"
#include "x86.hpp"
#include "icb.hpp"
#include "gc.hpp"
#include "jvm_context.hpp"

static ICB::Loader s_loader;
static Java::CachingClassLoader s_cacheLoader;
static Java::VM s_vm;

static void initMemory()
{
	Log::info("Initialising memory");
	if (!Multiboot::hasMultibootFlag(MULTIBOOT_INFO_MEM_MAP))
	{
		Log::critical("No memory map provided");
		Arch::panic();
	}

	multiboot_memory_map_t* entry = reinterpret_cast<multiboot_memory_map_t*>(Multiboot::mbt->mmap_addr - sizeof(u32));
	while (entry->type != MULTIBOOT_MEMORY_AVAILABLE)
		entry++;
	s_vm.gc().init(reinterpret_cast<u8*>(entry->addr), entry->len);

	if (!Multiboot::hasMultibootFlag(MULTIBOOT_INFO_MODS) || Multiboot::mbt->mods_count == 0)
	{
		Log::critical("No modules found");
		Arch::panic();
	}
}

static void initICB()
{
	Log::info("Initialising ICB");
	size_t moduleCount = Multiboot::mbt->mods_count;
	Log::infof("Found %d modules", moduleCount);

	multiboot_module_t* mmod = reinterpret_cast<multiboot_module_t*>(Multiboot::mbt->mods_addr);
	Multiboot::Module module(mmod);

	Log::infof("Found module at %x with size of %d bytes", module.data, module.length);

	ICB::Bundle bundle = ICB::Bundle::from(module.data, module.length);

	if (!bundle.isICB1())
	{
		Log::critical("Module is not an ICB1 file");
		Arch::panic();
	}

	ICB::Entry entry;
	size_t count = 0;
	while (bundle.next(entry))
	{
		switch (entry.tag)
		{
		case ICB::Tag::CLASS:
			Log::infof("Found class file '%s'", entry.name);
			count++;
			break;
		default:
			Log::warning("Found invalid tag in ICB bundle");
			break;
		}
	}
	Log::infof("Found %d classes in the ICB bundle", count);

	s_loader.bundle(bundle);
}

static void initVM()
{
	Log::info("Initialising VM");
	s_cacheLoader.parent(s_loader);
	s_vm.init(s_loader, nullptr, 0);
}

extern "C" void arch_main(multiboot_info_t* mbt)
{
	Multiboot::mbt = mbt;

	Arch::init_serial();
	initMemory();
	initICB();
	initVM();

	GC::Root<char> startupClassName;
	s_vm.allocateString(startupClassName, "raviolios/Startup");

	Log::info("Loading startup class");
	GC::Root<Java::ClassFile> startupClass;
//	(void) s_cacheLoader.loadClass(s_vm, startupClass, startupClassName);
	Log::info("A");
	Java::ClassError error = s_vm.getClass(startupClass, startupClassName);
	Log::info("B");
	if (error != Java::ClassError::GOOD)
	{
		Log::criticalf("Failed to load raviolios.Startup: %s", Java::toString(error));
		Arch::panic();
	}
	Log::info("Startup class loaded");

	GC::Root<Java::Thread> thread;
	Java::ThreadCreateResult result = s_vm.createThread(thread, startupClass, "startup");
	if (result != Java::ThreadCreateResult::CREATED)
	{
		Log::criticalf("Could not create thread: %s", Java::toString(result));
		Arch::panic();
	}
	Log::info("Thread created");

	while (s_vm.step(thread) == Java::ThreadState::RUNNING)
	{
	}
	Log::critical("Thread stopped");

	Log::critical("Reached end of arch_main");
	Arch::panic();
}
