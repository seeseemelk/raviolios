/**
 * @file
 * @brief
 */

#ifndef _JVM_OPCODES_HPP_
#define _JVM_OPCODES_HPP_

#include "defs.hpp"
#include "gc.hpp"

namespace Java
{
//	enum class Opcode : u8
//	{
//		ldc = 0x12,
//		istore_0 = 0x3B,
//		istore_1 = 0x3C,
//		istore_2 = 0x3D,
//		istore_3 = 0x3E,
//		astore_0 = 0x4B,
//		astore_1 = 0x4C,
//		astore_2 = 0x4D,
//		astore_3 = 0x4E,
//		dup = 0x59,
//		swap = 0x5F,
//		iadd = 0x60,
////		ddiv = 0x6F,
//		iinc = 0x84,
////		if_icmpeq = 0x9f,
//		if_icmpne = 0xA0,
//		if_icmplt = 0xA1,
////		if_icmpge = 0xA2,
////		if_icmpgt = 0xA3,
////		if_icmple = 0xA4,
//		goto_ = 0xA7,
//		ireturn = 0xAC,
//		getstatic = 0xB2,
//		putstatic = 0xB3,
//		putfield = 0xB5,
//		new_ = 0xBB,
//	};

	enum class Opcode
	{
		iconst,
		aload,
		iload,
		getfield_a,
		getfield_b,
		getfield_c,
//		getfield_byte,
//		getfield_short,
//		getfield_integer,
//		getfield_object,
		invoke_a,
		invoke_b,
		invoke_native,
		invoke_special,
		return_value,
		return_,
	};
}

#endif
