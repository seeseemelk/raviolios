#include "io.hpp"

using namespace IO;

void IO::outb(Port port, u8 value)
{
	asm ("outb %1, %0" :: "d" (port), "a" (value));
}

void IO::outw(Port port, u16 value)
{
	asm ("outw %1, %0" :: "d" (port), "a" (value));
}

void IO::outl(Port port, u32 value)
{
	asm ("outl %1, %0" :: "d" (port), "a" (value));
}

u8 IO::inb(Port port)
{
	u8 value;
	asm volatile ("inb %1, %0" : "=a" (value) : "d" (port));
	return value;
}

u16 IO::inw(Port port)
{
	u16 value;
	asm volatile ("inw %1, %0" : "=a" (value) : "d" (port));
	return value;
}

u32 IO::inl(Port port)
{
	u32 value;
	asm volatile ("inl %1, %0" : "=a" (value) : "d" (port));
	return value;
}


