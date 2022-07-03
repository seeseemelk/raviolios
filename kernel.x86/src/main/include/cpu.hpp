/*
 * cpu.h
 *
 *  Created on: Jul 3, 2022
 *      Author: seeseemelk
 */

#ifndef KERNEL_X86_SRC_MAIN_INCLUDE_CPU_HPP_
#define KERNEL_X86_SRC_MAIN_INCLUDE_CPU_HPP_

#include "defs.hpp"

extern "C"
{
	void cli();
	void sti();
	u32 getCR2();
	void reloadPageTable();
}

#endif /* KERNEL_X86_SRC_MAIN_INCLUDE_CPU_HPP_ */
