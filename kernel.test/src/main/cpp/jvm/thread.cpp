#include "asserts.hpp"

#include "cut.hpp"
#include "fileloader.hpp"
#include "jvm_context.hpp"

using namespace Java;

TEST("Can run a thread")
{
	CUT cut;

	GC::Root<Thread> thread;
	GC::Root<ClassFile> classfile;
	cut.createThread(thread);
	cut.loadClass(classfile, thread, "StaticReturnsInt");
	ThreadCreateResult result = cut.vm.createThread(thread, classfile, "returns1");
	assertEquals(ThreadCreateResult::CREATED, result, "Thread was created");

	int steps = 10;

	ThreadState state = cut.vm.step(thread);
	assertEquals(ThreadState::RUNNING, state, "Thread is running");

	while (steps > 0 && state == ThreadState::RUNNING)
	{
		state = cut.vm.step(thread);
		steps--;
	}

	assertEquals(ThreadState::STOPPED, state, "Thread has stopped");
}

static bool s_called = false;

static void setCalledToTrue(VM& /*vm*/, GC::Root<Thread>& /*thread*/)
{
	s_called = true;
}

TEST("Can call a native method")
{
	NativeMethod nativeMethods[] = {
			{
					.className = "Tests",
					.methodName = "nativeTest",
					.methodType = "()V",
					.method = setCalledToTrue
			}
	};

	CUT cut(nativeMethods, 1);

	GC::Root<Thread> thread;
	GC::Root<ClassFile> classfile;
	cut.createThread(thread);
	assertEquals(ClassError::GOOD, cut.loadClass(classfile, thread, "Tests"), "Class file was loaded successfully");
	ThreadCreateResult result = cut.vm.createThread(thread, classfile, "callNative");
	assertEquals(ThreadCreateResult::CREATED, result, "Thread was created");

	s_called = false;
	ThreadState state;
	do
	{
		state = cut.vm.step(thread);
	}
	while (state == ThreadState::RUNNING);
	assertTrue(s_called, "Native method was called");
}
