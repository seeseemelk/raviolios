#include "util.hpp"

bool equals(const GC::Root<char>& stringA, const char* stringB)
{
	size_t len = length(stringB);
	if (len != stringA.object->size)
		return false;
	const char* charsA = stringA.asPtr();
	for (size_t i = 0; i < len; i++)
	{
		if (charsA[i] != stringB[i])
			return false;
	}
	return true;
}

bool equals(const GC::Root<char>& stringA, const GC::Root<char>& stringB)
{
	if (stringB.object->size != stringA.object->size)
		return false;
	size_t length = stringB.object->size;

	const char* charsA = stringA.asPtr();
	const char* charsB = stringB.asPtr();
	for (size_t i = 0; i < length; i++)
	{
		if (charsA[i] != charsB[i])
			return false;
	}
	return true;
}

size_t length(const char* string)
{
	size_t length = 0;
	while (true)
	{
		if (string[length] == '\0')
			return length;
		length++;
	}
}
