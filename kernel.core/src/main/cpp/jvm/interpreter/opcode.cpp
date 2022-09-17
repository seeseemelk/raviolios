#include "jvm_opcodes.hpp"

using namespace Java;

const char* Java::toString(Opcode opcode)
{
	switch (opcode)
	{
	case Opcode::nop: return "nop";
	case Opcode::panic: return "panic";
	case Opcode::dup: return "dup";
	case Opcode::dup_x1: return "dup_x1";
	case Opcode::swap: return "swap";
	case Opcode::pop: return "pop";
	case Opcode::load_constant: return "load_constant";
	case Opcode::load_string: return "load_string";
	case Opcode::create_string_a: return "create_string_a";
	case Opcode::create_string_b: return "create_string_b";
	case Opcode::aconst: return "aconst";
	case Opcode::iconst: return "iconst";
	case Opcode::load: return "load";
	case Opcode::store: return "store";
	case Opcode::array_load_byte: return "array_load_byte";
	case Opcode::array_load_char: return "array_load_char";
	case Opcode::array_store_char: return "array_store_char";
	case Opcode::iadd: return "iadd";
	case Opcode::isub: return "isub";
	case Opcode::idiv: return "idiv";
	case Opcode::imul: return "imul";
	case Opcode::ineg: return "ineg";
	case Opcode::irem: return "irem";
	case Opcode::iinc: return "iinc";
	case Opcode::i2b: return "i2b";
	case Opcode::i2c: return "i2c";
	case Opcode::new_a: return "new_a";
	case Opcode::new_b: return "new_b";
	case Opcode::new_c: return "new_c";
	case Opcode::newarray: return "newarray";
	case Opcode::arraylength: return "arraylength";
	case Opcode::checkcast_a: return "checkcast_a";
	case Opcode::checkcast_b: return "checkcast_b";
	case Opcode::goto_a: return "goto_a";
	case Opcode::goto_b: return "goto_b";
	case Opcode::ifeq_a: return "ifeq_a";
	case Opcode::ifeq_b: return "ifeq_b";
	case Opcode::ifne_a: return "ifne_a";
	case Opcode::ifne_b: return "ifne_b";
	case Opcode::ifle_a: return "ifle_a";
	case Opcode::ifle_b: return "ifle_b";
	case Opcode::ifge_a: return "ifge_a";
	case Opcode::ifge_b: return "ifge_b";
	case Opcode::if_icmpne_a: return "if_icmpne_a";
	case Opcode::if_icmpne_b: return "if_icmpne_b";
	case Opcode::if_icmpge_a: return "if_icmpge_a";
	case Opcode::if_icmpge_b: return "if_icmpge_b";
	case Opcode::if_acmpeq_a: return "if_acmpeq_a";
	case Opcode::if_acmpeq_b: return "if_acmpeq_b";
	case Opcode::if_acmpne_a: return "if_acmpne_a";
	case Opcode::if_acmpne_b: return "if_acmpne_b";
	case Opcode::ifnull_a: return "ifnull_a";
	case Opcode::ifnull_b: return "ifnull_b";
	case Opcode::ifnonnull_a: return "ifnonnull_a";
	case Opcode::ifnonnull_b: return "ifnonnull_b";
	case Opcode::getstatic_a: return "getstatic_a";
	case Opcode::getstatic_b: return "getstatic_b";
	case Opcode::getstatic_c: return "getstatic_c";
	case Opcode::putstatic_a: return "putstatic_a";
	case Opcode::putstatic_b: return "putstatic_b";
	case Opcode::putstatic_c: return "putstatic_c";
	case Opcode::getfield_a: return "getfield_a";
	case Opcode::getfield_b: return "getfield_b";
	case Opcode::getfield_c: return "getfield_c";
	case Opcode::putfield_a: return "putfield_a";
	case Opcode::putfield_b: return "putfield_b";
	case Opcode::putfield_c: return "putfield_c";
	case Opcode::invoke_a: return "invoke_a";
	case Opcode::invoke_b: return "invoke_b";
	case Opcode::invoke_native: return "invoke_native";
	case Opcode::invoke_special: return "invoke_special";
	case Opcode::invokevirtual_a: return "invokevirtual_a";
	case Opcode::invokevirtual_b: return "invokevirtual_b";
	case Opcode::invokevirtual_c: return "invokevirtual_c";
	case Opcode::return_value: return "return_value";
	case Opcode::return_: return "return";
	}
	return "?";
}
