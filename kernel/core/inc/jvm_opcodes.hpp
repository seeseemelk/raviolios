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
		bipush = 0x10,
		sipush = 0x11,
		iload_0 = 0x1A,
		iload_1 = 0x1B,
		iload_2 = 0x1C,
		iload_3 = 0x1D,
//		ddiv = 0x6F,
//		if_icmpeq = 0x9f,
		if_icmpne = 0xA0,
		if_icmplt = 0xA1,
//		if_icmpge = 0xA2,
//		if_icmpgt = 0xA3,
//		if_icmple = 0xA4,
		goto_ = 0xA7,
		return_ = 0xB1,
		getstatic = 0xB2,
		invokestatic = 0xB8,
		ireturn = 0xAC,
	};
}

#endif
