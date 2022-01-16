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
		iconst_0 = 0x03,
		iconst_1 = 0x04,
		iconst_2 = 0x05,
		iconst_3 = 0x06,
		iconst_4 = 0x07,
		iconst_5 = 0x08,
		return_ = 0xB1,
		invokestatic = 0xB8,
		ireturn = 0xAC,
	};
}

#endif
