#include "util.hpp"

static bool equals(const char* stringA, size_t lengthA, const char* stringB, size_t lengthB)
{
	if (lengthA != lengthB)
		return false;
	for (size_t i = 0; i < lengthA; i++)
	{
		if (stringA[i] != stringB[i])
			return false;
	}
	return true;
}

bool equals(const GC::Root<char>& stringA, const char* stringB)
{
	return equals(
			stringA.asPtr(), stringA.object->size,
			stringB, stringLength(stringB)
	);
}

bool equals(const GC::Root<char>& stringA, const GC::Root<char>& stringB)
{
	return equals(
			stringA.asPtr(), stringA.object->size,
			stringB.asPtr(), stringB.object->size
	);
}

bool equals(const GC::Array<char>* stringA, const char* stringB)
{
	return equals(
			stringA->asPtr(), stringA->size,
			stringB, stringLength(stringB)
	);
}

bool equals(const GC::Array<char>* stringA, const GC::Array<char>* stringB)
{
	return equals(
			stringA->asPtr(), stringA->size,
			stringB->asPtr(), stringB->size
	);
}

bool equals(const GC::Root<char>& stringA, const GC::Array<char>* stringB)
{
	return equals(
			stringA.asPtr(), stringA.object->size,
			stringB->asPtr(), stringB->size
	);
}

size_t stringLength(const char* string)
{
	size_t length = 0;
	while (true)
	{
		if (string[length] == '\0')
			return length;
		length++;
	}
}
