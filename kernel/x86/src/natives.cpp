#include "natives.hpp"

#include "log.hpp"

static void printHelloWorld()
{
	Log::info("Hello world from Java!");
}

const Java::NativeMethod g_nativeMethods[] =
{
	{
		.className = "raviolios/Startup",
		.methodName = "printHelloWorld",
		.methodType = "()V",
		.method = &printHelloWorld
	}
};

const size_t g_nativeMethodsCount = sizeof(g_nativeMethods) / sizeof(Java::NativeMethod);
