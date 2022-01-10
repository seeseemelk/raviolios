#include "log.hpp"

#include "arch.hpp"

void Log::trace(const char* line)
{
	(void) line;
//	Arch::log("[TRCE] ");
//	Arch::log(line);
//	Arch::log("\n");
}

void Log::info(const char* line)
{
	Arch::log("[INFO] ");
	Arch::log(line);
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

