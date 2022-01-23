#include "log.hpp"

#include "arch.hpp"
#include "defs.hpp"

extern "C" {
#include <stdarg.h>
}

static void printNumberRecurse(int number)
{
	if (number == 0)
		return;
	printNumberRecurse(number / 10);
	Arch::log('0' + number % 10);
}

static void printNumber(int number)
{
	if (number == 0)
	{
		Arch::log('0');
		return;
	}
	if (number < 0)
	{
		Arch::log('-');
		number = -number;
	}
	printNumberRecurse(number);
}

static void printHex(u32 number, size_t byteLength)
{
	size_t nibbles = byteLength * 2;
	size_t shift = 4 * (nibbles - 1);

	for (size_t i = 0; i < nibbles; i++)
	{
		u8 nibble = number >> shift;
		if (nibble < 10)
			Arch::log(nibble + '0');
		else
			Arch::log(nibble - 10 + 'A');
		number <<= 4;
	}
}

static void printFormatted(const char* fmt, va_list args)
{
	bool escaped = false;
	while (*fmt != '\0')
	{
		if (escaped)
		{
			switch (*fmt)
			{
			case 'x':
				Arch::log("0x");
				printHex(va_arg(args, u32), 4);
				break;
			case 'b':
				Arch::log("0x");
				printHex(va_arg(args, u32), 1);
				break;
			case 'd':
				printNumber(va_arg(args, int));
				break;
			case 's':
				Arch::log(va_arg(args, char*));
				break;
			case '%':
				Arch::log('%');
				break;
			}
			escaped = false;
		}
		else if (*fmt == '%')
		{
			escaped = true;
		}
		else
		{
			Arch::log(*fmt);
		}
		fmt++;
	}
}

void Log::trace(const char* line)
{
//	(void) line;
	Arch::log("[TRCE] ");
	Arch::log(line);
	Arch::log("\n");
}

void Log::info(const char* line)
{
	Arch::log("[INFO] ");
	Arch::log(line);
	Arch::log("\n");
}

void Log::infof(const char* fmt, ...)
{
	Arch::log("[INFO] ");
	va_list args;
	va_start(args, fmt);
	printFormatted(fmt, args);
	va_end(args);
	Arch::log("\n");
}

void Log::warning(const char* line)
{
	Arch::log("[WARN] ");
	Arch::log(line);
	Arch::log("\n");
}

void Log::warningf(const char* fmt, ...)
{
	Arch::log("[WARN] ");
	va_list args;
	va_start(args, fmt);
	printFormatted(fmt, args);
	va_end(args);
	Arch::log("\n");
}

void Log::error(const char* line)
{
	Arch::log("[ ERR] ");
	Arch::log(line);
	Arch::log("\n");
}

void Log::critical(const char* line)
{
	Arch::log("[CRIT] ");
	Arch::log(line);
	Arch::log("\n");
}

void Log::criticalf(const char* fmt, ...)
{
	Arch::log("[CRIT] ");
	va_list args;
	va_start(args, fmt);
	printFormatted(fmt, args);
	va_end(args);
	Arch::log("\n");
}

