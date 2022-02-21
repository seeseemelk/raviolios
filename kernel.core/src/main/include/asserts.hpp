/**@file
 * @brief
 */
#ifndef _ASSERTS_H_
#define _ASSERTS_H_

#include "test.hpp"

#include "gc.hpp"

#include <cstddef>

void assertTrue(bool value, const char* message);
void assertFalse(bool value, const char* message);

void assertStringEqual(const char* expected, const char* actual, size_t length, const char* message);
void assertStringEqual(const char* expected, const GC::Array<char>* actual, const char* message);
void assertStringEqual(const char* expected, const GC::Root<char>& actual, const char* message);

template<typename A, typename B>
void assertEquals(A expected, B actual, const char* message)
{
	assertTrue(expected == actual, message);
}

template<typename A, typename B>
void assertNotEquals(A expected, B actual, const char* message)
{
	assertTrue(expected != actual, message);
}

template<typename T>
void assertNull(T actual, const char* message)
{
	assertTrue(actual == nullptr, message);
}

template<typename T>
void assertNotNull(T actual, const char* message)
{
	assertTrue(actual != nullptr, message);
}

#endif /* _ASSERTS_H_ */
