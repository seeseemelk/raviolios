#include "jvm_context.hpp"
#include "jvm_thread.hpp"

#include "arch.hpp"
#include "log.hpp"
#include "util.hpp"

using namespace Java;

void Operand::describer(GC::Meta* /*object*/, GC::MetaVisitor& /*visitor*/)
{
}

Opcode Frame::getOpcode(size_t index)
{
	return code->object.getOpcode(index);
}

u16 Frame::getU16FromCode(size_t index)
{
	u16 high = static_cast<u16>(getOpcode(index).opcode);
	u16 low = static_cast<u16>(getOpcode(index + 1).opcode);
	return (high << 8) | low;
}

GC::Object<ClassFile>* Frame::getClassFile()
{
	return methodInfo->object.classFile;
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
	visitor.visit(&frame->methodInfo);
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

	GC::Root<MethodInfo> method;
	m_gc.makeRoot(classfile.get().methods->get(methodIndex).method, method);
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

void VM::createFrame(GC::Root<Frame>& frame, GC::Root<MethodInfo>& method)
{
	CodeAttribute& attribute = method.get().getAttributeOfType(AttributeType::code)->code->object;
	u16 localSize = attribute.maxLocals;
	u16 stackSize = attribute.maxStack;

	GC::Allocator<Frame> frameAllocator(Frame::describer);
	m_gc.allocate(frameAllocator, frame);
	frame.get().pc = 0;
	frame.get().previous = nullptr;
	method.store(&frame.get().methodInfo);

	GC::Allocator<Operand> localAllocator(localSize, Operand::describer);
	GC::Allocator<Operand> stackAllocator(stackSize, Operand::describer);

	GC::Root<Operand> local;
	GC::Root<Operand> stack;
	m_gc.allocate(localAllocator, local);
	m_gc.allocate(stackAllocator, stack);

	local.store(&frame.get().locals);
	stack.store(&frame.get().stack);
	frame.get().code = method.get().getAttributeOfType(AttributeType::code)->code;
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
	case Instruction::iconst_0:
		pushInteger(frame.get(), 0);
		goto valid_opcode;
	case Instruction::iconst_1:
		pushInteger(frame.get(), 1);
		goto valid_opcode;
	case Instruction::iconst_2:
		pushInteger(frame.get(), 2);
		goto valid_opcode;
	case Instruction::iconst_3:
		pushInteger(frame.get(), 3);
		goto valid_opcode;
	case Instruction::iconst_4:
		pushInteger(frame.get(), 4);
		goto valid_opcode;
	case Instruction::iconst_5:
		pushInteger(frame.get(), 5);
		goto valid_opcode;
	case Instruction::return_:
		returnFromMethod(thread);
		goto valid_opcode;
	case Instruction::ireturn:
		returnInteger(thread);
		goto valid_opcode;
	case Instruction::invokestatic:
		invokeStatic(thread);
		goto valid_opcode;
	}

//invalid_opcode:
	Log::critical("Invalid opcode");
	Arch::panic();
valid_opcode:
	return ThreadState::RUNNING;
}

void VM::pushInteger(Frame& frame, i32 number)
{
	Operand operand;
	operand.integer = number;
	frame.push(operand);
}

void VM::returnFromMethod(GC::Root<Thread>& thread)
{
	Frame& topFrame = thread.get().top->object;
	thread.get().top = topFrame.previous;
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

void VM::invokeStatic(GC::Root<Thread>& thread)
{
	GC::Root<Frame> frame;
	m_gc.makeRoot(thread.get().top, frame);
	u16 methodIndex = frame.get().getU16FromCode(frame.get().pc) - 1;
	frame.get().pc += 2;

	ClassFile& classFile = frame.get().getClassFile()->object;
	ConstantPoolMethodRef methodRef = classFile.constantPool->get(methodIndex).c_method;

	GC::Root<char> className;
	u16 classNameIndex = classFile.constantPool->get(methodRef.classIndex).c_class.nameIndex;
	m_gc.makeRoot(classFile.constantPool->get(classNameIndex).c_utf8.bytes, className);

	GC::Root<char> methodName;
	u16 methodNameIndex = classFile.constantPool->get(methodRef.nameAndTypeIndex).c_nameAndType.nameIndex;
	m_gc.makeRoot(classFile.constantPool->get(methodNameIndex).c_utf8.bytes, methodName);

	GC::Root<char> methodType;
	u16 methodTypeIndex = classFile.constantPool->get(methodRef.nameAndTypeIndex).c_nameAndType.descriptorIndex;
	m_gc.makeRoot(classFile.constantPool->get(methodTypeIndex).c_utf8.bytes, methodType);

	GC::Root<ClassFile> targetClass;
	ClassError error = getClass(targetClass, className);
	if (error != ClassError::GOOD)
	{
		Log::error("Error while loading class");
		Arch::panic();
	}

	u16 methodIndexInTargetClass = targetClass.get().findMethodByNameAndType(methodName, methodType);
	GC::Root<MethodInfo> targetMethod;
	m_gc.makeRoot(targetClass.get().methods->get(methodIndexInTargetClass).method, targetMethod);

	if (targetMethod.get().isNative())
	{
		invokeNativeMethod(className, methodName, methodType);
	}
	else
	{
		GC::Root<Frame> newFrame;
		createFrame(newFrame, targetMethod);
		frame.store(&newFrame.get().previous);
		newFrame.store(&thread.get().top);
	}
}

void VM::invokeNativeMethod(const GC::Root<char>& className, const GC::Root<char>& methodName, const GC::Root<char>& methodType)
{
	for (size_t i = 0; i < m_nativeMethodCount; i++)
	{
		NativeMethod& nativeMethod = m_nativeMethods[i];
		if (!equals(className, nativeMethod.className))
			continue;
		if (!equals(methodName, nativeMethod.methodName))
			continue;
		if (!equals(methodType, nativeMethod.methodType))
			continue;
		nativeMethod.method();
	}
}






