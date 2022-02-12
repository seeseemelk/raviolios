#include "jvm_thread.hpp"

using namespace Java;

GC::Object<ClassFile>* Frame::getClassFile()
{
	return methodInfo->object.classFile;
}

void Frame::push(Java::Operand& operand)
{
	if (stackIndex == stack->count())
	{
		Log::critical("Stack is full");
		Arch::panic();
	}
	stack->get(stackIndex) = operand;
	stackIndex++;
}

Operand Frame::pop()
{
	if (stackIndex == 0)
	{
		Log::critical("Stack is empty");
		Arch::panic();
	}
	stackIndex--;
	return stack->get(stackIndex);
}

void Frame::describer(GC::Meta* object, GC::MetaVisitor& visitor)
{
	Frame* frame = object->as<Frame>();
	visitor.visit(&frame->methodInfo);
	visitor.visit(&frame->previous);
	visitor.visit(&frame->locals);
	visitor.visit(&frame->stack);
	visitor.visit(&frame->instructions);
}
