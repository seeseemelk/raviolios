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
	void warningf(const char* fmt, ...);
	void error(const char* line);
	void critical(const char* line);
	void criticalf(const char* fmt, ...);
}

#endif /* _LOG_HPP_ */
