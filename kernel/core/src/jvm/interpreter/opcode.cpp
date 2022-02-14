#include "jvm_opcodes.hpp"

using namespace Java;

const char* Java::toString(Opcode opcode)
{
	switch (opcode)
	{
	case Opcode::panic: return "panic";
	case Opcode::dup: return "dup";
	case Opcode::swap: return "swap";
	case Opcode::load_constant: return "load_constant";
	case Opcode::iconst: return "iconst";
	case Opcode::load: return "load";
	case Opcode::store: return "store";
	case Opcode::iadd: return "iadd";
	case Opcode::iinc: return "iinc";
	case Opcode::new_a: return "new_a";
	case Opcode::new_b: return "new_b";
	case Opcode::new_c: return "new_c";
	case Opcode::goto_a: return "goto_a";
	case Opcode::goto_b: return "goto_b";
	case Opcode::if_icmpne_a: return "if_icmpne_a";
	case Opcode::if_icmpne_b: return "if_icmpne_b";
	case Opcode::getstatic_a: return "getstatic_a";
	case Opcode::getstatic_b: return "getstatic_b";
	case Opcode::getstatic_c: return "getstatic_c";
	case Opcode::putstatic_a: return "putstatic_a";
	case Opcode::putstatic_b: return "putstatic_b";
	case Opcode::putstatic_c: return "putstatic_c";
	case Opcode::getfield_a: return "getfield_a";
	case Opcode::getfield_b: return "getfield_b";
	case Opcode::getfield_byte: return "getfield_byte";
	case Opcode::getfield_short: return "getfield_short";
	case Opcode::getfield_int: return "getfield_int";
	case Opcode::getfield_long: return "getfield_long";
	case Opcode::getfield_object: return "getfield_object";
	case Opcode::putfield_a: return "putfield_a";
	case Opcode::putfield_b: return "putfield_b";
	case Opcode::putfield_byte: return "putfield_byte";
	case Opcode::putfield_short: return "putfield_short";
	case Opcode::putfield_int: return "putfield_int";
	case Opcode::putfield_long: return "putfield_long";
	case Opcode::putfield_object: return "putfield_object";
	case Opcode::invoke_a: return "invoke_a";
	case Opcode::invoke_b: return "invoke_b";
	case Opcode::invoke_native: return "invoke_native";
	case Opcode::invoke_special: return "invoke_special";
	case Opcode::return_value: return "return_value";
	case Opcode::return_: return "return";
	}
	return "?";
}
