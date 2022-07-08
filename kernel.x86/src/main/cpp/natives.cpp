#include "natives.hpp"

#include "arch.hpp"
#include "defs.hpp"
#include "log.hpp"
#include "math.hpp"
#include "paging.hpp"

using namespace Java;

static void printHelloWorld(VM& /*vm*/, GC::Root<Thread>& /*thread*/)
{
	Log::info("Hello world from Java!");
}

static void trace(VM& /*vm*/, GC::Root<Thread>& thread)
{
	Operand operand = thread.get().top->object.pop();
	char chr = operand.integer;
	Arch::log(chr);
}

static u32 s_mappedStart = 0xFF800000;
static void mapMemory(VM& vm, GC::Root<Thread>& thread)
{
	size_t size = Math::ceilg(thread.get().top->object.pop().integer, KB(4));
	u32 address = thread.get().top->object.pop().integer;
	s_mappedStart -= size;

	size_t maps = size / KB(4);
	for (size_t i = 0; i < maps; i++)
	{
		size_t offset = i * KB(4);
		Paging::PageEntry& entry = Paging::getPageEntryForAddress(vm.gc(), s_mappedStart + offset);
		entry.setAddress(address + offset);
		entry.rw = 1;
		entry.present = 1;
	}

	Operand result;
	result.integer = s_mappedStart;
	thread.get().top->object.push(result);
}

static void poke(VM& /*vm*/, GC::Root<Thread>& thread)
{
	u8 value = thread.get().top->object.pop().integer;
	u32 address = thread.get().top->object.pop().integer;
	u8* memory = reinterpret_cast<u8*>(address);
	*memory = value;
}

static void freeBytes(VM& vm, GC::Root<Thread>& thread)
{
	Operand result;
	result.integer = static_cast<i32>(vm.gc().getFree());
	thread.get().top->object.push(result);
}

static void usedBytes(VM& vm, GC::Root<Thread>& thread)
{
	Operand result;
	result.integer = static_cast<i32>(vm.gc().getUsed());
	thread.get().top->object.push(result);
}

const Java::NativeMethod g_nativeMethods[] =
{
	{
		.className = "raviolios/Startup",
		.methodName = "printHelloWorld",
		.methodType = "()V",
		.method = &printHelloWorld
	},
	{
		.className = "raviolios/Arch",
		.methodName = "trace",
		.methodType = "(C)V",
		.method = &trace
	},
	{
		.className = "raviolios/Arch",
		.methodName = "mapMemory",
		.methodType = "(II)I",
		.method = &mapMemory
	},
	{
		.className = "raviolios/Arch",
		.methodName = "poke",
		.methodType = "(IB)V",
		.method = &poke
	},
	{
		.className = "raviolios/Arch",
		.methodName = "freeBytes",
		.methodType = "()I",
		.method = &freeBytes
	},
	{
		.className = "raviolios/Arch",
		.methodName = "usedBytes",
		.methodType = "()I",
		.method = &usedBytes
	}
};

const size_t g_nativeMethodsCount = sizeof(g_nativeMethods) / sizeof(Java::NativeMethod);
