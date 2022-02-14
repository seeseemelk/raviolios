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
//		astore_0 = 0x4B,
//		astore_1 = 0x4C,
//		astore_2 = 0x4D,
//		astore_3 = 0x4E,
//		dup = 0x59,
//		swap = 0x5F,
////		ddiv = 0x6F,
////		if_icmpeq = 0x9f,
//		if_icmplt = 0xA1,
////		if_icmpge = 0xA2,
////		if_icmpgt = 0xA3,
////		if_icmple = 0xA4,
//		ireturn = 0xAC,
//		new_ = 0xBB,
//	};

	enum class Opcode : u16
	{
		panic,
		load_constant,
		iconst,
		load,
		store,
		iadd,
		iinc,
		goto_a,
		goto_b,
		if_icmpne_a,
		if_icmpne_b,
		getstatic_a,
		getstatic_b,
		getstatic_c,
		putstatic_a,
		putstatic_b,
		putstatic_c,
		getfield_a,
		getfield_b,
		getfield_c,
		putfield_a,
		putfield_b,
		putfield_c,
		invoke_a,
		invoke_b,
		invoke_native,
		invoke_special,
		return_value,
		return_,
	};

	const char* toString(Opcode opcode);
}

#endif
