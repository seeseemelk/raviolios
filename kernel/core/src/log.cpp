#include "log.hpp"

#include "arch.hpp"

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

static void printFormatted(const char* fmt, va_list args)
{
	bool escaped = false;
	while (*fmt != '\0')
	{
		if (escaped)
		{
			switch (*fmt)
			{
			case 'd':
				printNumber(va_arg(args, int));
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

