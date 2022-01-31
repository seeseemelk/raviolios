#ifndef _ARCH_HPP_
#define _ARCH_HPP_

/**
 * Namespace for stuff that needs to be accessible by everything, but has to be
 * implemented by the architecture itself.
 */
namespace Arch
{
	/**
	 * Log a zero-terminated string.
	 * @param text The text to log.
	 */
	void log(const char* text);

	/**
	 * Log a single character string.
	 * @param chr The character to log.
	 */
	void log(char chr);

	/**
	 * Makes the system do whatever panic function it wants.
	 * It may, for instance:
	 *  - Hang forever
	 *  - Reboot
	 *  - Pause for a debugger
	 */
	[[noreturn]]
	void panic();
}

#endif /* _ARCH_HPP_ */
