#ifndef IO_HPP_
#define IO_HPP_

#include "defs.hpp"

namespace IO
{
	class Port
	{
	public:
		void set_port(u16 port);
		void outb(u8 value) const;
		void outw(u16 value) const;
		void outl(u32 value) const;
		u8 inb() const;
		u16 inw() const;
		u32 inl() const;

		void outb(int value) const;
		void outw(int value) const;
	private:
		u16 m_port;
	};
}

#endif /* IO_HPP_ */
