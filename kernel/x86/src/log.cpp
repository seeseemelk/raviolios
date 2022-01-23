#include "arch.hpp"

#include "io.hpp"
#include "x86.hpp"

#define SERIAL_BASE 0x3F8
#define DATA_AVAILABLE (1<<0)
#define TRANSMITTER_EMPTY (1<<1)
#define BREAK_ERROR (1<<2)
#define STATUS_CHANGE (1<<3)

static IO::Port reg_data;
static IO::Port reg_ie;
static IO::Port reg_fifo;
static IO::Port reg_line_ctrl;
static IO::Port reg_modem_ctrl;
static IO::Port reg_line_status;

void Arch::init_serial()
{
	reg_data.set_port(SERIAL_BASE);
	reg_ie.set_port(SERIAL_BASE + 1);
	reg_fifo.set_port(SERIAL_BASE + 2);
	reg_line_ctrl.set_port(SERIAL_BASE + 3);
	reg_modem_ctrl.set_port(SERIAL_BASE + 4);
	reg_line_status.set_port(SERIAL_BASE + 5);

	// Disable interrupts
	reg_ie.outb(0x00);

	// Enable DLAB
	reg_line_ctrl.outb(0x80);

	// Set baudrate to 38400
	reg_data.outb(0x03);
	reg_ie.outb(0x00);

	// Set 8 bits, no parity, one stop bit
	reg_line_ctrl.outb(0x03);

	// Enable FIFO and clear
	reg_fifo.outb(0xC7);

	// IRQs enabled, RTS/DSR set
	reg_modem_ctrl.outb(0x0B);
}

static int is_transmit_empty()
{
	return reg_line_status.inb() & 0x20;
}

static void put(const char c)
{
	while (!is_transmit_empty());
	reg_data.outb(c);
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

