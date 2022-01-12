#include "jvm_context.hpp"
#include "jvm_thread.hpp"

#include "arch.hpp"
#include "log.hpp"

using namespace Java;

void Operand::describer(GC::Meta* /*object*/, GC::MetaVisitor& /*visitor*/)
{
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

void VM::createThread(GC::Root<Thread>& thread, const GC::Root<ClassFile>& classfile, u16 methodIndex)
{
	GC::Allocator<Thread> allocator(Thread::describer);
	m_gc.allocate(allocator, thread);

	MethodInfo& method = classfile.get().methods->get(methodIndex);
	GC::Root<Frame> frame;
	createFrame(frame, method);
	frame.store(&thread.get().top);
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
}

ThreadState VM::step(GC::Root<Thread>& thread)
{
	if (thread.get().top == nullptr)
		return ThreadState::ENDED;

	GC::Root<Frame> frame;
	m_gc.makeRoot(thread.get().top, frame);

	u16 pc = frame.get().pc;
	frame.get().pc = pc + 1;

	u8 opcode = frame.get().code->object.code->get(pc).opcode;
	switch (opcode)
	{
	default:
		Log::critical("Invalid opcode");
		Arch::panic();
		break;
	}

	return ThreadState::RUNNING;
}




//Thread::Thread(VM& vm, Method& method)
//	: m_vm(vm)
//{
//}
