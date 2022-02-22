#include "fileloader.hpp"

#include <fstream>
#include <iostream>

Buffer::~Buffer()
{
	if (data != nullptr)
	{
		delete[] data;
		data = nullptr;
	}
}

bool loadFile(Buffer& buf, std::string& file)
{
	std::ifstream input(file);
	if (!input)
		return false;

	input.seekg(0, std::ios_base::end);
	size_t pos = input.tellg();
	buf.data = new u8[pos];
	buf.length = pos;

	input.seekg(0);
	input.read(reinterpret_cast<char*>(buf.data), buf.length);
	return true;
}

bool loadTestClass(Buffer& buf, std::string file)
{
	std::string path = "build/classes/java/main/" + file + ".class";
	if (loadFile(buf, path))
		return true;
	std::string corePath = "../kernel.core/build/classes/java/main/" + file + ".class";
	return loadFile(buf, corePath);
}
