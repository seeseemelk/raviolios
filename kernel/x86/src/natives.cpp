#include "natives.hpp"

#include "arch.hpp"
#include "log.hpp"

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
	}
};

const size_t g_nativeMethodsCount = sizeof(g_nativeMethods) / sizeof(Java::NativeMethod);
