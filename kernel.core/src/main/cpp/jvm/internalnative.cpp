#include "jvm_internalnative.hpp"

#include "defs.hpp"

using namespace Java;

static void totalBytes(VM& vm, GC::Root<Thread>& thread)
{
	Operand operand;
	operand.integer = static_cast<i32>(vm.gc().getCapacity());
	thread.get().top->object.push(operand);
}

const Java::NativeMethod Java::g_internalNativeMethods[] = {
		{
				.className = "raviolios/Arch",
				.methodName = "totalBytes",
				.methodType = "()I",
				.method = totalBytes
		}
};
const size_t Java::g_internalNativeMethodsCount = sizeof(g_internalNativeMethods) / sizeof(Java::NativeMethod);
