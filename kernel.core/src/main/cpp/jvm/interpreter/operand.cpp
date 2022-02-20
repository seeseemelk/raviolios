#include "jvm_thread.hpp"

using namespace Java;

void Operand::describer(GC::Meta* object, GC::MetaVisitor& visitor)
{
	Operand* operands = object->as<Operand>();
	size_t count = object->size / sizeof(Operand);
	for (size_t i = 0; i < count; i++)
	{
		if (operands[i].isObject)
			visitor.visit(&operands[i].object);
	}
}
