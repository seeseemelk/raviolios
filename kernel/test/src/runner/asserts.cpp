#include "asserts.hpp"

#include <cstring>

void assertTrue(bool value, const char* message)
{
	if (value == true)
		Test::succeed(message);
	else
		Test::fail(message);
}

void assertFalse(bool value, const char* message)
{
	assertTrue(!value, message);
}

void assertStringEqual(const char* expected, const char* actual, size_t length, const char* message)
{
	if (strlen(expected) != length)
		Test::fail(message);
	if (memcmp(expected, actual, length) != 0)
		Test::fail(message);
}

void assertStringEqual(const char* expected, const GC::Array<char>* actual, const char* message)
{
	assertStringEqual(expected, actual->asPtr(), actual->size, message);
}

void assertStringEqual(const char* expected, const GC::Root<char>& actual, const char* message)
{
	assertStringEqual(expected, actual.asPtr(), actual.object->size, message);
}
