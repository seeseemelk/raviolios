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
////		ddiv = 0x6F,
////		if_icmpeq = 0x9f,
//		if_icmplt = 0xA1,
////		if_icmpge = 0xA2,
////		if_icmpgt = 0xA3,
////		if_icmple = 0xA4,
//		ireturn = 0xAC,
//	};

	enum class Opcode : u16
	{
		panic,
		dup,
		swap,
		load_constant,
		iconst,
		load,
		store,
		iadd,
		iinc,
		new_a,
		new_b,
		new_c,
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
		getfield_byte,
		getfield_short,
		getfield_int,
		getfield_long,
		getfield_object,
		putfield_a,
		putfield_b,
		putfield_byte,
		putfield_short,
		putfield_int,
		putfield_long,
		putfield_object,
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
