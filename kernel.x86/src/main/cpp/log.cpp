#include "arch.hpp"

#include "io.hpp"
#include "x86.hpp"

#define SERIAL_BASE 0x3F8
#define DATA_AVAILABLE (1<<0)
#define TRANSMITTER_EMPTY (1<<1)
#define BREAK_ERROR (1<<2)
#define STATUS_CHANGE (1<<3)

#define REG_DATA (SERIAL_BASE)
#define REG_IE (SERIAL_BASE + 1)
#define REG_FIFO (SERIAL_BASE + 2)
#define REG_LINE_CTRL (SERIAL_BASE + 3)
#define REG_MODEM_CTRL (SERIAL_BASE + 4)
#define REG_LINE_STATUS (SERIAL_BASE + 5)

void Arch::init_serial()
{
	// Disable interrupts
	IO::outb(REG_IE, 0x00);

	// Enable DLAB
	IO::outb(REG_LINE_CTRL, 0x80);

	// Set baudrate to 38400
	IO::outb(REG_DATA, 0x03);
	IO::outb(REG_IE, 0x00);

	// Set 8 bits, no parity, one stop bit
	IO::outb(REG_LINE_CTRL, 0x03);

	// Enable FIFO and clear
	IO::outb(REG_FIFO, 0xC7);

	// IRQs enabled, RTS/DSR set
	IO::outb(REG_MODEM_CTRL, 0x0B);
}

static int is_transmit_empty()
{
	return IO::inb(REG_LINE_STATUS) & 0x20;
}

static void put(const char c)
{
	while (!is_transmit_empty());
	IO::outb(REG_DATA, c);
}

void Arch::log(char chr)
{
	put(chr);
}

void Arch::log(const char* text)
{
	while (*text != 0)
	{
		put(*text);
		text++;
	}
}

