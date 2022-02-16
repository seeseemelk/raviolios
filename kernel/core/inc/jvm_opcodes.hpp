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
		i2b,
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
		getfield_c,
		putfield_a,
		putfield_b,
		putfield_c,
		invoke_a,
		invoke_b,
		invoke_native,
		invoke_special,
		invokevirtual_a,
		invokevirtual_b,
		invokevirtual_c,
		return_value,
		return_,
	};

	const char* toString(Opcode opcode);
}

#endif
