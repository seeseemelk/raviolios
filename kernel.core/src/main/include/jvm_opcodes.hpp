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

		/**
		 * Pushes the string object that is stored by the instruction onto the
		 * stack.
		 */
		load_string,

		/**
		 * Creates a new instance of the type `string`, and push it onto the
		 * stack twice.
		 * It then calls `String.fromUtf8` with the UTF-8 string as a byte
		 * array.
		 */
		create_string_a,

		/**
		 * Takes the string instance that is on the stack, and stores it into
		 * the constant pool.
		 * Then, it becomes the `load_string` instruction.
		 */
		create_string_b,
		iconst,
		load,
		store,
		array_load_byte,
		array_load_char,
		array_store_char,
		iadd,
		imul,
		iinc,
		i2b,
		i2c,
		new_a,
		new_b,
		new_c,
		newarray,
		arraylength,
		goto_a,
		goto_b,
		if_icmpne_a,
		if_icmpne_b,
		if_icmpge_a,
		if_icmpge_b,
		if_acmpne_a,
		if_acmpne_b,
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
