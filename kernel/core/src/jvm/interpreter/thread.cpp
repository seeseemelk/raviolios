#include "jvm_thread.hpp"

#include "arch.hpp"
#include "jvm_context.hpp"
#include "jvm_type.hpp"
#include "log.hpp"
#include "util.hpp"

using namespace Java;

//Opcode Frame::getOpcode(size_t index)
//{
//	return code->object.getOpcode(index);
//}
//
//u8 Frame::getU8FromCode(size_t index)
//{
//	return static_cast<u16>(getOpcode(index).opcode);
//}
//
//u16 Frame::getU16FromCode(size_t index)
//{
//	u16 high = getU8FromCode(index);
//	u16 low = getU8FromCode(index + 1);
//	return (high << 8) | low;
//}
//
//static void dumpStack(Frame& frame)
//{
//	Log::infof("Stack dump (length %d):", frame.stack->count());
//	for (size_t i = 0; i < frame.stack->count(); i++)
//	{
//		Log::infof("[%d] %d", i, frame.stack->get(i).integer);
//	}
//}

void Thread::describer(GC::Meta* object, GC::MetaVisitor& visitor)
{
	Thread* thread = object->as<Thread>();
	visitor.visit(&thread->top);
}

void VM::createThread(GC::Root<Thread>& thread)
{
	GC::Allocator<Thread> allocator(Thread::describer);
	m_gc.allocate(allocator, thread);
}

ThreadCreateResult VM::createThread(GC::Root<Thread>& thread, const GC::Root<ClassFile>& classfile, u16 methodIndex)
{
	if (methodIndex > classfile.get().methodCount)
		return ThreadCreateResult::NO_METHOD;

	createThread(thread);
	invokeMethod(thread, classfile, methodIndex);

	/*GC::Root<MethodInfo> method;
	m_gc.makeRoot(classfile.get().methods->get(methodIndex).method, method);
	GC::Root<Frame> frame;
	createFrame(frame, method);
	frame.store(&thread.get().top);*/
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
	u16 stackSize = attribute.maxStack + 1;

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
	frame.get().instructions = attribute.instructions;
	frame.get().stackIndex = 0;
}

ThreadState VM::step(GC::Root<Thread>& thread)
{
	m_interrupted = true;
	return runUntilInterrupted(thread);
}

ThreadState VM::run(GC::Root<Thread>& thread)
{
	m_interrupted = false;
	return runUntilInterrupted(thread);
}

ThreadState VM::runUntilInterrupted(GC::Root<Thread>& thread)
{
	if (thread.get().top == nullptr)
		return ThreadState::STOPPED;

	GC::Root<Frame> frame;
	m_gc.makeRoot(thread.get().top, frame);

	u16 pc = frame.get().pc;
	do
	{
		Instruction instruction = frame.get().instructions->get(pc);
		bool storeInstruction = false;
		bool returnFromFrame = false;
		switch (instruction.opcode)
		{
		case Opcode::iconst:
			opcodeIconst(frame, instruction.constantInteger);
			pc++;
			break;
		case Opcode::aload:
			opcodeAload(frame, instruction.index);
			pc++;
			break;
		case Opcode::iload:
			opcodeIload(frame, instruction.index);
			pc++;
			break;
		case Opcode::getfield_a:
			instruction = opcodeGetfieldA(thread, frame, instruction.index);
			storeInstruction = true;
			break;
		case Opcode::getfield_b:
			instruction = opcodeGetfieldB(frame, instruction.index);
			storeInstruction = true;
			break;
		case Opcode::getfield_c:
			opcodeGetfieldC(frame, instruction.targetField);
			pc++;
			break;
		case Opcode::invoke_a:
			instruction = opcodeInvokeA(thread, frame, instruction.index);
			storeInstruction = true;
			break;
		case Opcode::invoke_b:
			instruction = opcodeInvokeB(frame, instruction.index);
			storeInstruction = true;
			break;
		case Opcode::invoke_native:
			instruction.targetNativeMethod(*this, thread);
			pc++;
			break;
		case Opcode::invoke_special:
			opcodeInvokeSpecial(thread, frame, instruction.targetMethod);
			frame.get().pc = pc + 1;
			m_gc.makeRoot(thread.get().top, frame);
			pc = frame.get().pc;
			break;
		case Opcode::return_value:
			opcodeReturnValue(thread, frame);
			returnFromFrame = true;
			break;
		case Opcode::return_:
			opcodeReturn(thread, frame);
			returnFromFrame = true;
			break;
		}
		if (storeInstruction)
		{
			frame.get().instructions->get(pc) = instruction;
		}
		if (returnFromFrame)
		{
			if (thread.get().top == nullptr)
				return ThreadState::STOPPED;
			m_gc.makeRoot(thread.get().top, frame);
		}
	}
	while (!m_interrupted);
	frame.get().pc = pc;
	return ThreadState::RUNNING;
}

void VM::opcodeIconst(GC::Root<Frame>& frame, i32 value)
{
	Operand operand;
	operand.integer = value;
	frame.get().push(operand);
}

void VM::opcodeAload(GC::Root<Frame>& frame, u16 index)
{
	Operand operand = frame.get().locals->get(index);
	frame.get().push(operand);
}

void VM::opcodeIload(GC::Root<Frame>& frame, u16 index)
{
	Operand operand = frame.get().locals->get(index);
	frame.get().push(operand);
}

Instruction VM::opcodeGetfieldA(GC::Root<Thread>& thread, GC::Root<Frame>& frame, u16 index)
{
	ClassFile& classFile = frame.get().getClassFile()->object;
	ConstantPoolFieldRef fieldRef = classFile.constantPool->get(index).c_field;

	GC::Root<char> className;
	u16 classNameIndex = classFile.constantPool->get(fieldRef.classIndex).c_class.nameIndex;
	m_gc.makeRoot(classFile.constantPool->get(classNameIndex).c_utf8.bytes, className);

	findClass(thread, frame, className);

	Instruction result;
	result.opcode = Opcode::getfield_b;
	result.index = index;
	return result;
}

Instruction VM::opcodeGetfieldB(GC::Root<Frame>& frame, u16 index)
{
	ClassFile& classFile = frame.get().getClassFile()->object;
	ConstantPoolFieldRef fieldRef = classFile.constantPool->get(index).c_field;

	GC::Root<char> fieldName;
	u16 fieldNameIndex = classFile.constantPool->get(fieldRef.nameAndTypeIndex).c_nameAndType.nameIndex;
	m_gc.makeRoot(classFile.constantPool->get(fieldNameIndex).c_utf8.bytes, fieldName);

	Operand operand = frame.get().pop();
	GC::Object<ClassFile>* targetClass = operand.classFile;

	u16 fieldIndex = targetClass->object.findFieldByName(fieldName);

	Instruction instruction;
	instruction.opcode = Opcode::getfield_c;
	instruction.targetField = targetClass->object.fields->get(fieldIndex).field;
//	switch (instruction.targetField->object.type.type)
//	{
//	case Type::BOOLEAN:
//	case Type::BYTE:
//	case Type::CHAR:
//		instruction.opcode = Opcode::getfield_byte;
//		break;
//	case Type::SHORT:
//		instruction.opcode = Opcode::getfield_short;
//		break;
//	case Type::INTEGER:
//	case Type::FLOAT:
//		instruction.opcode = Opcode::getfield_integer;
//		break;
//	case Type::REFERENCE:
//		instruction.opcode = Opcode::getfield_object;
//		break;
//	case Type::VOID:
//	case Type::LONG:
//	case Type::DOUBLE:
//		Arch::panic();
//		break;
//	}
	return instruction;
}

void VM::opcodeGetfieldC(GC::Root<Frame>& frame, GC::Object<FieldInfo>* field)
{
	Operand& operand = field->object.value;
	frame.get().push(operand);
}

Instruction VM::opcodeInvokeA(GC::Root<Thread>& thread, GC::Root<Frame>& frame, u16 index)
{
	ClassFile& classFile = frame.get().getClassFile()->object;
	ConstantPoolMethodRef methodRef = classFile.constantPool->get(index).c_method;

	GC::Root<char> className;
	u16 classNameIndex = classFile.constantPool->get(methodRef.classIndex).c_class.nameIndex;
	m_gc.makeRoot(classFile.constantPool->get(classNameIndex).c_utf8.bytes, className);

	findClass(thread, frame, className);

	Instruction result;
	result.opcode = Opcode::invoke_b;
	result.index = index;
	return result;
}

Instruction VM::opcodeInvokeB(GC::Root<Frame>& frame, u16 index)
{
	ClassFile& classFile = frame.get().getClassFile()->object;
	ConstantPoolMethodRef methodRef = classFile.constantPool->get(index).c_method;

	GC::Root<char> methodName;
	u16 methodNameIndex = classFile.constantPool->get(methodRef.nameAndTypeIndex).c_nameAndType.nameIndex;
	m_gc.makeRoot(classFile.constantPool->get(methodNameIndex).c_utf8.bytes, methodName);

	GC::Root<char> methodType;
	u16 methodTypeIndex = classFile.constantPool->get(methodRef.nameAndTypeIndex).c_nameAndType.descriptorIndex;
	m_gc.makeRoot(classFile.constantPool->get(methodTypeIndex).c_utf8.bytes, methodType);

	Operand operand = frame.get().pop();
	GC::Object<ClassFile>* targetClass = operand.classFile;

	u16 methodIndexInTargetClass = targetClass->object.findMethodByNameAndType(methodName, methodType);

	GC::Object<MethodInfo>* method = targetClass->object.methods->get(methodIndexInTargetClass).method;

	Instruction result;
	if (method->object.isNative())
	{
		result.opcode = Opcode::invoke_native;
		result.targetNativeMethod = nullptr;

		GC::Root<char> className;
		u16 classNameIndex = classFile.constantPool->get(methodRef.classIndex).c_class.nameIndex;
		m_gc.makeRoot(classFile.constantPool->get(classNameIndex).c_utf8.bytes, className);

		for (size_t i = 0; i < m_nativeMethodCount; i++)
		{
			const NativeMethod& nativeMethod = m_nativeMethods[i];
			if (!equals(className, nativeMethod.className))
				continue;
			if (!equals(methodName, nativeMethod.methodName))
				continue;
			if (!equals(methodType, nativeMethod.methodType))
				continue;
			result.targetNativeMethod = nativeMethod.method;
		}
		if (result.targetNativeMethod == nullptr)
		{
			Log::criticalf("Unknown native method");
			Arch::panic();
		}
	}
	else
	{
		result.opcode = Opcode::invoke_special;
		result.targetMethod = method;
	}
	return result;
}

void VM::opcodeInvokeSpecial(GC::Root<Thread>& thread, GC::Root<Frame>& frame, GC::Object<MethodInfo>* method)
{
	GC::Root<MethodInfo> targetMethod;
	m_gc.makeRoot(method, targetMethod);
	invokeMethod(thread, targetMethod, frame.get().inInterrupt);
}

void VM::opcodeReturnValue(GC::Root<Thread>& thread, GC::Root<Frame>& frame)
{
	Operand value = frame.get().pop();
	thread.get().top = frame.get().previous;
	if (thread.get().top != nullptr)
		thread.get().top->object.push(value);
}

void VM::opcodeReturn(GC::Root<Thread>& thread, GC::Root<Frame>& frame)
{
	thread.get().top = frame.get().previous;
}

void VM::findClass(GC::Root<Thread>& thread, GC::Root<Frame>& frame, const GC::Root<char>& name)
{
	GC::Root<ClassFile> methodClass;
	m_gc.makeRoot(frame.get().getClassFile(), methodClass);
	if (methodClass.get().classLoader == nullptr)
	{
		GC::Root<ClassFile> targetClass;
		ClassError error = getClass(targetClass, thread, name);
		if (error != ClassError::GOOD)
		{
			Log::criticalf("Failed to load class: %s", toString(error));
			Arch::panic();
		}
		Operand operand;
		targetClass.store(&operand.classFile);
		frame.get().push(operand);
	}
	else
	{
		Log::critical("Non-native classloader not yet supported");
		Arch::panic();
	}
}

//ThreadState VM::step(GC::Root<Thread>& thread)
//{
//	if (thread.get().top == nullptr)
//		return ThreadState::STOPPED;
//
//	GC::Root<Frame> frame;
//	m_gc.makeRoot(thread.get().top, frame);
//
//	u16 pc = frame.get().pc;
//	frame.get().pc = pc + 1;
//
//	Frame& frameL = frame.get();
//	CodeAttribute* codeAttribute = &frameL.code->object;
//	Instruction opcode = codeAttribute->code->get(pc).opcode;
//	Log::infof("Executing opcode %d (byte index: %d)", opcode, pc);
//	dumpStack(frame.get());
//	switch (opcode)
//	{
//	case Instruction::iconst_0:
//		pushInteger(frame.get(), 0);
//		goto valid_opcode;
//	case Instruction::iconst_1:
//		pushInteger(frame.get(), 1);
//		goto valid_opcode;
//	case Instruction::iconst_2:
//		pushInteger(frame.get(), 2);
//		goto valid_opcode;
//	case Instruction::iconst_3:
//		pushInteger(frame.get(), 3);
//		goto valid_opcode;
//	case Instruction::iconst_4:
//		pushInteger(frame.get(), 4);
//		goto valid_opcode;
//	case Instruction::iconst_5:
//		pushInteger(frame.get(), 5);
//		goto valid_opcode;
//	case Instruction::bipush:
//		pushByte(frame.get());
//		goto valid_opcode;
//	case Instruction::sipush: // 0x11
//		pushShort(frame.get());
//		goto valid_opcode;
//	case Instruction::ldc: // 0x12
//		pushConstant(frame.get());
//		goto valid_opcode;
//	case Instruction::iload_0: // 0x1A
//		pushIntegerFromVariable(frame.get(), 0);
//		goto valid_opcode;
//	case Instruction::iload_1:
//		pushIntegerFromVariable(frame.get(), 1);
//		goto valid_opcode;
//	case Instruction::iload_2:
//		pushIntegerFromVariable(frame.get(), 2);
//		goto valid_opcode;
//	case Instruction::iload_3:
//		pushIntegerFromVariable(frame.get(), 3);
//		goto valid_opcode;
//	case Instruction::aload_0: // 0x2A
//		pushReferenceFromVariable(frame.get(), 0);
//		goto valid_opcode;
//	case Instruction::aload_1: // 0x2A
//		pushReferenceFromVariable(frame.get(), 0);
//		goto valid_opcode;
//	case Instruction::aload_2: // 0x2A
//		pushReferenceFromVariable(frame.get(), 0);
//		goto valid_opcode;
//	case Instruction::aload_3: // 0x2A
//		pushReferenceFromVariable(frame.get(), 0);
//		goto valid_opcode;
//	case Instruction::istore_0: // 0x3B
//		storeIntegerToVariable(frame.get(), 0);
//		goto valid_opcode;
//	case Instruction::istore_1: // 0x3C
//		storeIntegerToVariable(frame.get(), 1);
//		goto valid_opcode;
//	case Instruction::istore_2: // 0x3D
//		storeIntegerToVariable(frame.get(), 2);
//		goto valid_opcode;
//	case Instruction::istore_3: // 0x3E
//		storeIntegerToVariable(frame.get(), 3);
//		goto valid_opcode;
//	case Instruction::astore_0: // 0x4B
//		storeReferenceToVariable(frame.get(), 0);
//		goto valid_opcode;
//	case Instruction::astore_1: // 0x4C
//		storeReferenceToVariable(frame.get(), 1);
//		goto valid_opcode;
//	case Instruction::astore_2: // 0x4D
//		storeReferenceToVariable(frame.get(), 2);
//		goto valid_opcode;
//	case Instruction::astore_3: // 0x4E
//		storeReferenceToVariable(frame.get(), 3);
//		goto valid_opcode;
//	case Instruction::dup: // 0x59
//		dup(frame.get());
//		goto valid_opcode;
//	case Instruction::swap: // 0x5F
//		swap(frame.get());
//		goto valid_opcode;
//	case Instruction::iadd: // 0x60
//		addIntegers(frame.get());
//		goto valid_opcode;
//	case Instruction::iinc: // 0x84
//		increment(frame.get());
//		goto valid_opcode;
//	case Instruction::if_icmpne: // 0xA0
//		jumpIfIntegerNotEqual(frame.get());
//		goto valid_opcode;
//	case Instruction::if_icmplt: // 0xA1
//		jumpIfIntegerLessThan(frame.get());
//		goto valid_opcode;
//	case Instruction::goto_: // 0xA7
//		jumpUnconditionally(frame.get());
//		goto valid_opcode;
//	case Instruction::ireturn: // 0xAC
//		returnInteger(thread);
//		goto valid_opcode;
//	case Instruction::return_: // 0xB1
//		returnFromMethod(thread);
//		goto valid_opcode;
//	case Instruction::getstatic: // 0xB2
//		getStatic(thread, frame);
//		goto valid_opcode;
//	case Instruction::putstatic: // 0xB3
//		putStatic(thread, frame);
//		goto valid_opcode;
//	case Instruction::getfield: // 0xB4
//		getField(thread, frame);
//		goto valid_opcode;
//	case Instruction::putfield: // 0xB5
//		putField(thread, frame);
//		goto valid_opcode;
//	case Instruction::invokespecial: // 0xB7
//		invokeSpecial(thread);
//		goto valid_opcode;
//	case Instruction::invokestatic: // 0xB8
//		invokeStatic(thread);
//		goto valid_opcode;
//	case Instruction::new_: // 0xBB
//		newObject(thread, frame);
//		goto valid_opcode;
//	}
//
////invalid_opcode:
//	Log::criticalf("Invalid opcode: %x", opcode);
//	Arch::panic();
//valid_opcode:
//	dumpStack(frame.get());
//	return ThreadState::RUNNING;
//}
//
//void VM::pushInteger(Frame& frame, i32 number)
//{
//	Operand operand;
//	operand.integer = number;
//	frame.push(operand);
//}
//
//void VM::pushReferenceFromVariable(Frame& frame, i32 number)
//{
//	Operand operand = frame.locals->get(number);
//	frame.push(operand);
//}
//
//void VM::pushIntegerFromVariable(Frame& frame, i32 number)
//{
//	Operand operand = frame.locals->get(number);
//	frame.push(operand);
//}
//
//void VM::storeIntegerToVariable(Frame& frame, i32 number)
//{
//	Operand operand = frame.pop();
//	frame.locals->get(number) = operand;
//}
//
//void VM::storeReferenceToVariable(Frame& frame, i32 number)
//{
//	Operand operand = frame.pop();
//	frame.locals->get(number) = operand;
//}
//
//void VM::pushShort(Frame& frame)
//{
//	i16 value = frame.getU16FromCode(frame.pc);
//	frame.pc += 2;
//	Operand operand;
//	operand.integer = value;
//	frame.push(operand);
//}
//
//void VM::pushByte(Frame& frame)
//{
//	i8 value = frame.getU8FromCode(frame.pc);
//	frame.pc++;
//	Operand operand;
//	operand.integer = value;
//	frame.push(operand);
//}
//
//void VM::pushConstant(Frame& frame)
//{
//	u16 index = frame.getU8FromCode(frame.pc) - 1;
//	frame.pc++;
//	ConstantPoolInfo& info = frame.getClassFile()->object.constantPool->get(index);
//	switch (info.tag)
//	{
//	case CONSTANT_integer:
//		pushInteger(frame, info.c_integer.integer);
//		break;
//	default:
//		Log::criticalf("Unknown constant pool tag: %d", info.tag);
//		break;
//	}
//}
//
//void VM::dup(Frame& frame)
//{
//	Operand operand = frame.pop();
//	frame.push(operand);
//	frame.push(operand);
//}
//
//void VM::swap(Frame& frame)
//{
//	Operand a = frame.pop();
//	Operand b = frame.pop();
//	frame.push(a);
//	frame.push(b);
//}
//
//void VM::addIntegers(Frame& frame)
//{
//	Operand a = frame.pop();
//	Operand b = frame.pop();
//	Operand result;
//	result.integer = a.integer + b.integer;
//	frame.push(result);
//}
//
//void VM::increment(Frame& frame)
//{
//	u8 index = frame.getU8FromCode(frame.pc++);
//	u8 amount = frame.getU8FromCode(frame.pc++);
//	frame.locals->get(index).integer += amount;
//}
//
//void VM::jumpIfIntegerNotEqual(Frame& frame)
//{
//	Operand b = frame.pop();
//	Operand a = frame.pop();
//	i16 target = frame.getU16FromCode(frame.pc) - 1;
//	Log::infof("A=%d, B=%d", a.integer, b.integer);
//	Log::infof("Target = %d", target);
//	if (a.integer != b.integer)
//		frame.pc += target;
//	else
//		frame.pc += 2;
//}
//
//void VM::jumpIfIntegerLessThan(Frame& frame)
//{
//	Operand b = frame.pop();
//	Operand a = frame.pop();
//	i16 target = frame.getU16FromCode(frame.pc) - 1;
//	if (a.integer < b.integer)
//		frame.pc += target;
//	else
//		frame.pc += 2;
//}
//
//void VM::jumpUnconditionally(Frame& frame)
//{
//	i16 target = frame.getU16FromCode(frame.pc) - 1;
//	frame.pc += target;
//}
//
//void VM::getStatic(GC::Root<Thread>& thread, GC::Root<Frame>& frame)
//{
//	(void) thread;
//	u16 index = frame.get().getU16FromCode(frame.get().pc) - 1;
//	frame.get().pc += 2;
////	Operand operand;
////	operand.integer = 0;
////	frame.get().push(operand);
//	(void) index;
//	GC::Root<ClassFile> classFile;
//	m_gc.makeRoot(frame.get().getClassFile(), classFile);
//
//	u16 targetClassIndex = classFile.get().constantPool->get(index).c_field.classIndex;
//	u16 targetFieldNameTypeIndex = classFile.get().constantPool->get(index).c_field.nameAndTypeIndex;
//
//	u16 targetClassNameIndex = classFile.get().constantPool->get(targetClassIndex).c_class.nameIndex;
//	u16 fieldNameIndex = classFile.get().constantPool->get(targetFieldNameTypeIndex).c_nameAndType.nameIndex;
//
//	GC::Root<char> targetClassName;
//	GC::Root<char> fieldName;
//
//	m_gc.makeRoot(classFile.get().constantPool->get(targetClassNameIndex).c_utf8.bytes, targetClassName);
//	m_gc.makeRoot(classFile.get().constantPool->get(fieldNameIndex).c_utf8.bytes, fieldName);
//
//	GC::Root<ClassFile> targetClass;
//	if (getClass(targetClass, thread, targetClassName) != ClassError::GOOD)
//	{
//		Log::critical("Could not find class");
//		Arch::panic();
//	}
//
//	u16 fieldIndex = targetClass.get().findFieldByName(fieldName);
//
//	frame.get().push(targetClass.get().fields->get(fieldIndex).value);
//}
//
//void VM::putStatic(GC::Root<Thread>& thread, GC::Root<Frame>& frame)
//{
//	(void) thread;
//	(void) frame;
//	u16 index = frame.get().getU16FromCode(frame.get().pc) - 1;
//	frame.get().pc += 2;
//	GC::Root<ClassFile> classFile;
//	(void) index;
//
//	Frame& frameA = frame.get();
//	GC::Object<ClassFile>* classFileA = frameA.getClassFile();
//	m_gc.makeRoot(classFileA, classFile);
//
//	u16 targetClassIndex = classFile.get().constantPool->get(index).c_field.classIndex;
//	u16 targetFieldNameTypeIndex = classFile.get().constantPool->get(index).c_field.nameAndTypeIndex;
//
//	u16 targetClassNameIndex = classFile.get().constantPool->get(targetClassIndex).c_class.nameIndex;
//	u16 fieldNameIndex = classFile.get().constantPool->get(targetFieldNameTypeIndex).c_nameAndType.nameIndex;
//
//	GC::Root<char> targetClassName;
//	GC::Root<char> fieldName;
//
//	m_gc.makeRoot(classFile.get().constantPool->get(targetClassNameIndex).c_utf8.bytes, targetClassName);
//	m_gc.makeRoot(classFile.get().constantPool->get(fieldNameIndex).c_utf8.bytes, fieldName);
//
//	GC::Root<ClassFile> targetClass;
//	if (getClass(targetClass, thread, targetClassName) != ClassError::GOOD)
//	{
//		Log::critical("Could not find class");
//		Arch::panic();
//	}
//
//	u16 fieldIndex = targetClass.get().findFieldByName(fieldName);
//
//	Operand operand = frame.get().pop();
//	targetClass.get().fields->get(fieldIndex).value = operand;
//}
//
//void VM::getField(GC::Root<Thread>& thread, GC::Root<Frame>& frame)
//{
//	frame.get().push(*findField(thread, frame));
//}
//
//void VM::putField(GC::Root<Thread>& thread, GC::Root<Frame>& frame)
//{
//	Operand value = frame.get().pop();
//	*findField(thread, frame) = value;
//}
//
//Operand* VM::findField(GC::Root<Thread>& thread, GC::Root<Frame>& frame)
//{
//	u16 fieldIndex = frame.get().getU16FromCode(frame.get().pc) - 1;
//	frame.get().pc += 2;
//
//	GC::Root<ClassFile> executingClass;
//	m_gc.makeRoot(frame.get().getClassFile(), executingClass);
//
//	ConstantPoolFieldRef fieldRef = executingClass.get().constantPool->get(fieldIndex).c_field;
//	ConstantPoolNameAndType fieldNameAndType = executingClass.get().constantPool->get(fieldRef.nameAndTypeIndex).c_nameAndType;
//
//	u16 targetClassNameIndex = executingClass.get().constantPool->get(fieldRef.classIndex).c_class.nameIndex;
//
//	GC::Root<char> targetClassName;
//	GC::Root<char> targetFieldName;
//
//	m_gc.makeRoot(executingClass.get().constantPool->get(targetClassNameIndex).c_utf8.bytes, targetClassName);
//	m_gc.makeRoot(executingClass.get().constantPool->get(fieldNameAndType.nameIndex).c_utf8.bytes, targetFieldName);
//
//	GC::Root<ClassFile> targetClass;
//	ClassError classError = getClass(targetClass, thread, targetClassName);
//	if (classError != ClassError::GOOD)
//	{
//		Log::criticalf("Failed to get class: %s", toString(classError));
//		Arch::panic();
//	}
//	u16 targetFieldIndex = targetClass.get().findFieldByName(targetFieldName);
//	i32 offset = targetClass.get().fields->get(targetFieldIndex).offset;
//
//	Operand object = frame.get().pop();
//	return object.object->object.getFieldAt<Operand>(offset);
//}
//
//void VM::returnFromMethod(GC::Root<Thread>& thread)
//{
//	Frame& topFrame = thread.get().top->object;
//	thread.get().top = topFrame.previous;
//}
//
//void VM::returnInteger(GC::Root<Thread>& thread)
//{
//	Frame& topFrame = thread.get().top->object;
//	Frame* previousFrame = &topFrame.previous->object;
//
//	if (topFrame.previous != nullptr)
//	{
//		Operand value = topFrame.pop();
//		previousFrame->push(value);
//	}
//
//	thread.get().top = topFrame.previous;
//}
//
//void VM::invokeSpecial(GC::Root<Thread>& thread)
//{
//	GC::Root<Frame> frame;
//	m_gc.makeRoot(thread.get().top, frame);
//	u16 methodIndex = frame.get().getU16FromCode(frame.get().pc) - 1;
//	frame.get().pc += 2;
//
//	ClassFile& classFile = frame.get().getClassFile()->object;
//	ConstantPoolMethodRef methodRef = classFile.constantPool->get(methodIndex).c_method;
//
//	GC::Root<char> className;
//	u16 classNameIndex = classFile.constantPool->get(methodRef.classIndex).c_class.nameIndex;
//	m_gc.makeRoot(classFile.constantPool->get(classNameIndex).c_utf8.bytes, className);
//
//	GC::Root<char> methodName;
//	u16 methodNameIndex = classFile.constantPool->get(methodRef.nameAndTypeIndex).c_nameAndType.nameIndex;
//	m_gc.makeRoot(classFile.constantPool->get(methodNameIndex).c_utf8.bytes, methodName);
//
//	GC::Root<char> methodType;
//	u16 methodTypeIndex = classFile.constantPool->get(methodRef.nameAndTypeIndex).c_nameAndType.descriptorIndex;
//	m_gc.makeRoot(classFile.constantPool->get(methodTypeIndex).c_utf8.bytes, methodType);
//
//	GC::Root<ClassFile> targetClass;
//	ClassError error = getClass(targetClass, thread, className);
//	if (error != ClassError::GOOD)
//	{
//		Log::error("Error while loading class");
//		Arch::panic();
//	}
//
//	u16 methodIndexInTargetClass = targetClass.get().findMethodByNameAndType(methodName, methodType);
//	invokeMethod(thread, targetClass, methodIndexInTargetClass);
//}
//
//void VM::invokeStatic(GC::Root<Thread>& thread)
//{
//	GC::Root<Frame> frame;
//	m_gc.makeRoot(thread.get().top, frame);
//	u16 methodIndex = frame.get().getU16FromCode(frame.get().pc) - 1;
//	frame.get().pc += 2;
//
//	ClassFile& classFile = frame.get().getClassFile()->object;
//	ConstantPoolMethodRef methodRef = classFile.constantPool->get(methodIndex).c_method;
//
//	GC::Root<char> className;
//	u16 classNameIndex = classFile.constantPool->get(methodRef.classIndex).c_class.nameIndex;
//	m_gc.makeRoot(classFile.constantPool->get(classNameIndex).c_utf8.bytes, className);
//
//	GC::Root<char> methodName;
//	u16 methodNameIndex = classFile.constantPool->get(methodRef.nameAndTypeIndex).c_nameAndType.nameIndex;
//	m_gc.makeRoot(classFile.constantPool->get(methodNameIndex).c_utf8.bytes, methodName);
//
//	GC::Root<char> methodType;
//	u16 methodTypeIndex = classFile.constantPool->get(methodRef.nameAndTypeIndex).c_nameAndType.descriptorIndex;
//	m_gc.makeRoot(classFile.constantPool->get(methodTypeIndex).c_utf8.bytes, methodType);
//
//	GC::Root<ClassFile> targetClass;
//	ClassError error = getClass(targetClass, thread, className);
//	if (error != ClassError::GOOD)
//	{
//		Log::error("Error while loading class");
//		Arch::panic();
//	}
//
//	u16 methodIndexInTargetClass = targetClass.get().findMethodByNameAndType(methodName, methodType);
//	invokeMethod(thread, targetClass, methodIndexInTargetClass);
//}

void VM::invokeNativeMethod(GC::Root<Thread>& thread, const GC::Root<char>& className, const GC::Root<char>& methodName, const GC::Root<char>& methodType)
{
	for (size_t i = 0; i < m_nativeMethodCount; i++)
	{
		const NativeMethod& nativeMethod = m_nativeMethods[i];
		if (!equals(className, nativeMethod.className))
			continue;
		if (!equals(methodName, nativeMethod.methodName))
			continue;
		if (!equals(methodType, nativeMethod.methodType))
			continue;
		nativeMethod.method(*this, thread);
		return;
	}
	Log::critical("No native method handler found");
	Arch::panic();
}

void VM::invokeMethod(GC::Root<Thread>& thread, const GC::Root<ClassFile>& targetClass, u16 method, bool isInterrupt)
{
	GC::Root<MethodInfo> targetMethod;
	m_gc.makeRoot(targetClass.get().methods->get(method).method, targetMethod);

	u16 classNameIndex = targetClass.get().constantPool->get(targetClass.get().thisClass).c_class.nameIndex;
	u16 methodNameIndex = targetMethod.get().nameIndex;
	u16 typeIndex = targetMethod.get().descriptorIndex;

	GC::Root<char> methodName;
	GC::Root<char> className;
	GC::Root<char> type;
	m_gc.makeRoot(targetClass.get().constantPool->get(methodNameIndex).c_utf8.bytes, methodName);
	m_gc.makeRoot(targetClass.get().constantPool->get(classNameIndex).c_utf8.bytes, className);
	m_gc.makeRoot(targetClass.get().constantPool->get(typeIndex).c_utf8.bytes, type);


	if (targetMethod.get().isNative())
	{
		invokeNativeMethod(thread, className, methodName, type);
	}
	else
	{
		GC::Root<Frame> frame;
		m_gc.makeRoot(thread.get().top, frame);

		GC::Root<Frame> newFrame;
		createFrame(newFrame, targetMethod);

		if (isInterrupt)
		{
			newFrame.get().inInterrupt = true;
			frame.store(&newFrame.get().previous);
			newFrame.store(&thread.get().top);
		}
		else
		{
			if (thread.get().top != nullptr && thread.get().top->object.inInterrupt)
			{
				GC::Object<Frame>* lastInt = thread.get().top;
				GC::Object<Frame>* firstNormal = lastInt->object.previous;
				while (firstNormal != nullptr && firstNormal->object.inInterrupt)
				{
					lastInt = firstNormal;
					firstNormal = firstNormal->object.previous;
				}
				frame.get().previous = firstNormal;
				newFrame.store(&lastInt->object.previous);
			}
			else
			{
				frame.store(&newFrame.get().previous);
				newFrame.store(&thread.get().top);
			}

			TypeDescriptor methodTypeDescriptor = TypeDescriptor::parse(type);
			size_t arguments = methodTypeDescriptor.arguments;
			if (!targetMethod.get().isStatic())
				arguments++;
			for (size_t i = arguments; i > 0; i--)
			{
				newFrame.get().locals->get(i - 1) = frame.get().pop();
			}
		}
	}
}

void VM::invokeMethod(GC::Root<Thread>& thread, GC::Root<MethodInfo>& targetMethod, bool isInterrupt)
{
	if (targetMethod.get().isNative())
	{
		Arch::panic();
		//invokeNativeMethod(thread, className, methodName, type);
	}
	else
	{
		GC::Root<Frame> frame;
		m_gc.makeRoot(thread.get().top, frame);

		GC::Root<Frame> newFrame;
		createFrame(newFrame, targetMethod);

		if (isInterrupt)
		{
			newFrame.get().inInterrupt = true;
			frame.store(&newFrame.get().previous);
			newFrame.store(&thread.get().top);
		}
		else
		{
			if (thread.get().top != nullptr && thread.get().top->object.inInterrupt)
			{
				GC::Object<Frame>* lastInt = thread.get().top;
				GC::Object<Frame>* firstNormal = lastInt->object.previous;
				while (firstNormal != nullptr && firstNormal->object.inInterrupt)
				{
					lastInt = firstNormal;
					firstNormal = firstNormal->object.previous;
				}
				frame.get().previous = firstNormal;
				newFrame.store(&lastInt->object.previous);
			}
			else
			{
				frame.store(&newFrame.get().previous);
				newFrame.store(&thread.get().top);
			}

			size_t arguments = targetMethod.get().type.arguments;
			if (!targetMethod.get().isStatic())
				arguments++;
			for (size_t i = arguments; i > 0; i--)
			{
				newFrame.get().locals->get(i - 1) = frame.get().pop();
			}
		}
	}
}

void VM::invokeMethod(GC::Root<Thread>& thread, const GC::Root<ClassFile>& classfile, const char* name)
{
	u16 methodIndexInTargetClass = classfile.get().findMethodByName(name);
	invokeMethod(thread, classfile, methodIndexInTargetClass, false);
}

//struct JavaObject
//{
//	GC::Object<ClassFile>* class_;
//
//	static void describer(GC::Meta* object, GC::MetaVisitor& visitor)
//	{
//		JavaObject* obj = object->as<JavaObject>();
//		visitor.visit(&obj->class_);
//	}
//};
//
//void VM::newObject(GC::Root<Thread>& thread, GC::Root<Frame>& frame)
//{
//	u16 classIndex = frame.get().getU16FromCode(frame.get().pc) - 1;
//	frame.get().pc += 2;
//
//	u16 classNameIndex = frame.get().getClassFile()->object.constantPool->get(classIndex).c_class.nameIndex;
//
//	GC::Root<char> className;
//	m_gc.makeRoot(frame.get().getClassFile()->object.constantPool->get(classNameIndex).c_utf8.bytes, className);
//
//	GC::Root<ClassFile> targetClass;
//	ClassError error = getClass(targetClass, thread, className);
//	if (error != ClassError::GOOD)
//	{
//		Log::criticalf("Failed to instantiate object: %s", toString(error));
//		Arch::panic();
//	}
//
//	GC::Allocator<JavaObject> objectAllocator(targetClass.get().objectSize, &JavaObject::describer);
//	GC::Root<JavaObject> object;
//
//	m_gc.allocate(objectAllocator, object);
//	targetClass.store(&object.get().class_);
//
//	Operand operand;
//	object.store(&operand.object);
//	operand.isObject = true;
//	frame.get().push(operand);
//}



























































