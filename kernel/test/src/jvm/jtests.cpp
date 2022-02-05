#include "asserts.hpp"
#include "cut.hpp"

using namespace Java;

static bool s_expectFail = false;
static bool s_failed = false;
static size_t s_assertsCalled = 0;
static void j_assertSuccess(VM& /*vm*/, GC::Root<Thread>& /*thread*/)
{
	s_assertsCalled++;
	Test::succeed("Success from Java");
}

static void j_assertFails(VM& /*vm*/, GC::Root<Thread>& /*thread*/)
{
	s_assertsCalled++;
	s_failed = true;
	if (s_expectFail)
		Test::succeed("Got expected assertion");
	else
		Test::fail("Failure from Java");
}

static Java::NativeMethod s_nativeMethods[] = {
		{
				.className = "raviolios/Asserts",
				.methodName = "success",
				.methodType = "()V",
				.method = &j_assertSuccess
		},
		{
				.className = "raviolios/Asserts",
				.methodName = "fail",
				.methodType = "()V",
				.method = &j_assertFails
		}
};
static size_t s_nativeMethodCount = sizeof(s_nativeMethods) / sizeof(Java::NativeMethod);

struct JCUT : CUT
{
	JCUT()
		: CUT(s_nativeMethods, s_nativeMethodCount)
	{
	}
};

static void runJavaTest(const char* className, const char* methodName)
{
	JCUT cut;
	GC::Root<Thread> thread;
	GC::Root<ClassFile> classfile;

	s_failed = false;
	cut.createThread(thread);
	ClassError error = cut.loadClass(classfile, thread, className);
	assertEquals(ClassError::GOOD, error, "Classes loaded correctly");

	ThreadCreateResult result = cut.vm.createThread(thread, classfile, methodName);
	assertEquals(ThreadCreateResult::CREATED, result, "Thread was created");

	s_assertsCalled = 0;
	size_t steps = 0;
	while (steps < 1000)
	{
		ThreadState state = cut.vm.step(thread);
		if (state == ThreadState::STOPPED)
			break;
		steps++;
	}
	assertTrue(s_assertsCalled > 0, "At least one assert was executed");
	if (s_expectFail)
	{
		assertEquals(true, s_failed, "Got at least one expected assertion");
	}
}

#define JAVA_TEST(className, methodName) \
	TEST("Java " className " " methodName) \
	{ \
		s_expectFail = false; \
		runJavaTest(className, methodName); \
	}

#define JAVA_FAIL(className, methodName) \
	TEST("Java " className " " methodName) \
	{ \
		s_expectFail = true; \
		runJavaTest(className, methodName); \
	}

JAVA_TEST("tests/Assertions", "succeeds");
JAVA_FAIL("tests/Assertions", "fails");
JAVA_TEST("tests/Assertions", "equalsSucceedsWhenSame");
JAVA_FAIL("tests/Assertions", "equalsFailsWhenDifferent");
JAVA_TEST("tests/StaticProperty", "propertyStartsAsZero");
JAVA_TEST("tests/StaticProperty", "propertyCanBeChanged");
JAVA_TEST("tests/StaticProperty", "propertiesHaveDefaultValues");
JAVA_TEST("tests/Constants", "canUseLargeIntegers");
JAVA_TEST("tests/Variables", "canSaveVariables");
JAVA_TEST("tests/Arithmetic", "canAddNumbers");
JAVA_TEST("tests/Arithmetic", "canIncrementNumbers");
