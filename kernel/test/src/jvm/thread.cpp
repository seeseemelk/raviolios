#include "asserts.hpp"

#include "cut.hpp"
#include "fileloader.hpp"
#include "jvm_context.hpp"

using namespace Java;

TEST("Can create a thread")
{
	CUT cut;

	GC::Root<Thread> thread;
	GC::Root<ClassFile> classfile;
	cut.loadClass(classfile, "Test1");
	ThreadCreateResult result = cut.vm.createThread(thread, classfile, "<init>");
	assertEquals(ThreadCreateResult::CREATED, result, "Thread was created");
}

TEST("Can run a thread")
{
	CUT cut;

	GC::Root<Thread> thread;
	GC::Root<ClassFile> classfile;
	cut.loadClass(classfile, "StaticReturnsInt");
	ThreadCreateResult result = cut.vm.createThread(thread, classfile, "returns1");
	assertEquals(ThreadCreateResult::CREATED, result, "Thread was created");

	ThreadState state = cut.vm.step(thread);
	assertEquals(ThreadState::RUNNING, state, "Thread is running");

	state = cut.vm.step(thread);
	assertEquals(ThreadState::RUNNING, state, "Thread is running");

	state = cut.vm.step(thread);
	assertEquals(ThreadState::STOPPED, state, "Thread has stopped");
}

static bool s_called = false;

static void setCalledToTrue()
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
	cut.loadClass(classfile, "Tests");
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
