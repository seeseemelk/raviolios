#include "jvm_thread.hpp"

using namespace Java;

void Instruction::describer(GC::Meta* object, GC::MetaVisitor& visitor)
{
	Instruction* instructions = object->as<Instruction>();
	size_t count = object->size / sizeof(Instruction);
	for (size_t i = 0; i < count; i++)
	{
		switch (instructions[i].opcode)
		{
		case Opcode::new_c:
			visitor.visit(&instructions[i].targetClass);
			break;
		case Opcode::invoke_special:
			visitor.visit(&instructions[i].targetMethod);
			break;
		case Opcode::getstatic_c:
		case Opcode::putstatic_c:
		case Opcode::getfield_c:
		case Opcode::putfield_c:
			visitor.visit(&instructions[i].targetField);
			break;
		case Opcode::load_string:
			visitor.visit(&instructions[i].targetObject);
			break;
		default:
			break;
		}
	}
}
