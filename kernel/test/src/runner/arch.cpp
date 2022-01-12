#include "arch.hpp"

#include <iostream>

void Arch::log(const char* text)
{
	std::cout << text;
}

void Arch::panic()
{
	abort();
}
