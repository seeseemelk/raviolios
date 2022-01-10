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

void loadFile(Buffer& buf, std::string& file)
{
	std::ifstream input(file);

	input.seekg(0, std::ios_base::end);
	size_t pos = input.tellg();
	buf.data = new u8[pos];
	buf.length = pos;

	input.seekg(0);
	input.read(reinterpret_cast<char*>(buf.data), buf.length);
}

void loadTestClass(Buffer& buf, std::string file)
{
	std::string path = "bin/test/kernel/test/java/" + file + ".class";
	loadFile(buf, path);
}
