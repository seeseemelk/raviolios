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

/**
 * Compares two strings for equality.
 *
 * @param stringA The first string.
 * @param stringB The second string.
 *
 * @return `true` if both strings are equal to each other, `false` if they are
 * different from each other.
 */
bool equals(const GC::Root<char>& stringA, const GC::Root<char>& stringB);

/**
 * Gets the length of a string.
 *
 * @param string The string to get the length of.
 *
 * @return The length of the string, in bytes.
 */
size_t length(const char* string);


#endif /* _JVM_UTIL_HPP_ */
