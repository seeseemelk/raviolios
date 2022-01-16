/**@file
 * @brief
 */
#ifndef _JVM_UTIL_HPP_
#define _JVM_UTIL_HPP_

#include "gc.hpp"

/**
 * Compares two strings for equality.
 *
 * @param stringA The first string.
 * @param stringB The second string.
 *
 * @return `true` if both strings are equal to each other, `false` if they are
 * different from each other.
 */
bool equals(const GC::Root<char>& stringA, const char* stringB);
bool equals(const GC::Root<char>& stringA, const GC::Root<char>& stringB);
bool equals(const GC::Root<char>& stringA, const GC::Array<char>* stringB);
bool equals(const GC::Array<char>* stringA, const char* stringB);
bool equals(const GC::Array<char>* stringA, const GC::Array<char>* stringB);

/**
 * Gets the length of a string.
 *
 * @param string The string to get the length of.
 *
 * @return The length of the string, in bytes.
 */
size_t stringLength(const char* string);

template<typename T>
void memorySet(T* memory, T value, size_t size)
{
	for (size_t i = 0; i < size; i++)
		memory[i] = value;
}

template<typename T>
void memoryMove(T* to, T* from, size_t size)
{
	if (from > to)
	{
		for (size_t i = 0; i < size; i++)
			to[i] = from[i];
	}
	else
	{
		for (size_t i = size; i > 0; i--)
			to[i - 1] = from[i - 1];
	}
}


#endif /* _JVM_UTIL_HPP_ */
