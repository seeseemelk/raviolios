#ifndef IO_HPP_
#define IO_HPP_

#include "defs.hpp"

namespace IO
{
	typedef u16 Port;

	void outb(Port port, u8 value);
	void outw(Port port, u16 value);
	void outl(Port port, u32 value);
	u8 inb(Port port);
	u16 inw(Port port);
	u32 inl(Port port);
}

#endif /* IO_HPP_ */
