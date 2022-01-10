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

void assertStringEqual(const char* expected, const char* actual, const char* message)
{
	assertTrue(strcmp(expected, actual) == 0, message);
}
