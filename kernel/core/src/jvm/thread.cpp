#include "jvm_context.hpp"
#include "jvm_thread.hpp"

#include "arch.hpp"
#include "log.hpp"

using namespace Java;

void Operand::describer(GC::Meta* /*object*/, GC::MetaVisitor& /*visitor*/)
{
}

void Frame::push(Java::Operand& operand)
{
	stack->get(stackIndex) = operand;
	stackIndex++;
}

Java::Operand Frame::pop()
{
	stackIndex--;
	return stack->get(stackIndex);
}

void Frame::describer(GC::Meta* object, GC::MetaVisitor& visitor)
{
	Frame* frame = object->as<Frame>();
	visitor.visit(&frame->previous);
}

void Thread::describer(GC::Meta* object, GC::MetaVisitor& visitor)
{
	Thread* thread = object->as<Thread>();
	visitor.visit(&thread->top);
}

ThreadCreateResult VM::createThread(GC::Root<Thread>& thread, const GC::Root<ClassFile>& classfile, u16 methodIndex)
{
	if (methodIndex > classfile.get().methodCount)
		return ThreadCreateResult::NO_METHOD;

	GC::Allocator<Thread> allocator(Thread::describer);
	m_gc.allocate(allocator, thread);

	MethodInfo& method = classfile.get().methods->get(methodIndex);
	GC::Root<Frame> frame;
	createFrame(frame, method);
	frame.store(&thread.get().top);
	return ThreadCreateResult::CREATED;
}

ThreadCreateResult VM::createThread(GC::Root<Thread>& thread, const GC::Root<ClassFile>& classfile, const char* method)
{
	u16 methodIndex = classfile.get().findMethodByName(method);
	return createThread(thread, classfile, methodIndex);
}

void VM::createFrame(GC::Root<Frame>& frame, MethodInfo& method)
{
	CodeAttribute& attribute = method.getAttributeOfType(AttributeType::code)->code->object;
	u16 localSize = attribute.maxLocals;
	u16 stackSize = attribute.maxStack;

	GC::Allocator<Frame> frameAllocator(Frame::describer);
	m_gc.allocate(frameAllocator, frame);
	frame.get().pc = 0;
	frame.get().previous = nullptr;

	GC::Allocator<Operand> localAllocator(localSize, Operand::describer);
	GC::Allocator<Operand> stackAllocator(stackSize, Operand::describer);

	GC::Root<Operand> local;
	GC::Root<Operand> stack;
	m_gc.allocate(localAllocator, local);
	m_gc.allocate(stackAllocator, stack);

	local.store(&frame.get().locals);
	stack.store(&frame.get().stack);
	frame.get().code = method.getAttributeOfType(AttributeType::code)->code;
	frame.get().stackIndex = 0;
}

ThreadState VM::step(GC::Root<Thread>& thread)
{
	if (thread.get().top == nullptr)
		return ThreadState::STOPPED;

	GC::Root<Frame> frame;
	m_gc.makeRoot(thread.get().top, frame);

	u16 pc = frame.get().pc;
	frame.get().pc = pc + 1;

	CodeAttribute* codeAttribute = &frame.get().code->object;
	Instruction opcode = codeAttribute->code->get(pc).opcode;
	switch (opcode)
	{
	case Instruction::iconst_4:
		pushInteger(frame.get(), 4);
		break;
	case Instruction::ireturn:
		returnInteger(thread);
		break;
	default:
		Log::critical("Invalid opcode");
		Arch::panic();
		break;
	}

	return ThreadState::RUNNING;
}

void VM::pushInteger(Frame& frame, i32 number)
{
	Operand operand;
	operand.integer = number;
	frame.push(operand);
}

void VM::returnInteger(GC::Root<Thread>& thread)
{
	Frame& topFrame = thread.get().top->object;
	Frame* previousFrame = &topFrame.previous->object;

	if (topFrame.previous != nullptr)
	{
		Operand value = topFrame.pop();
		previousFrame->push(value);
	}

	thread.get().top = topFrame.previous;
}

