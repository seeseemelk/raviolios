/**
 * @file
 * @brief
 */

#ifndef _JVM_OPCODES_HPP_
#define _JVM_OPCODES_HPP_

#include "defs.hpp"

namespace Java
{
	enum class Instruction : u8
	{
		iconst_4 = 0x04,
		ireturn = 0xAC,
	};
}

#endif
