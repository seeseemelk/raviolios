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
		nop,
		panic,
		dup,
		dup_x1,
		swap,
		pop,
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
		aconst,
		iconst,
		load,
		store,
		array_load_byte,
		array_load_char,
		array_store_char,
		iadd,
		isub,
		idiv,
		imul,
		irem,
		ineg,
		iinc,
		i2b,
		i2c,
		new_a,
		new_b,
		new_c,
		newarray,
		arraylength,
		checkcast_a,
		checkcast_b,
		goto_a,
		goto_b,
		ifeq_a,
		ifeq_b,
		ifne_a,
		ifne_b,
		ifge_a,
		ifge_b,
		ifle_a,
		ifle_b,
		if_icmpne_a,
		if_icmpne_b,
		if_icmpge_a,
		if_icmpge_b,
		if_acmpeq_a,
		if_acmpeq_b,
		if_acmpne_a,
		if_acmpne_b,
		ifnull_a,
		ifnull_b,
		ifnonnull_a,
		ifnonnull_b,
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
