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
		case Opcode::invoke_special:
			visitor.visit(&instructions[i].targetMethod);
			break;
		default:
			break;
		}
	}
}
