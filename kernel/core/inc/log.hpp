#ifndef _LOG_HPP_
#define _LOG_HPP_

/**
 * Logging utilities.
 */
namespace Log
{
	void trace(const char* line);
	void info(const char* line);
	void infof(const char* fmt, ...);
	void warning(const char* line);
	void error(const char* line);
	void critical(const char* line);
}

#endif /* _LOG_HPP_ */
