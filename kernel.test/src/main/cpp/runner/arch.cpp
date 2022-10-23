#include "arch.hpp"

#include <iostream>
#include "d2c_raviolios.h"

void Arch::log(const char* text)
{
	std::cout << text;
}

void Arch::log(char text)
{
	std::cout << text;
}

void Arch::panic()
{
	abort();
}
