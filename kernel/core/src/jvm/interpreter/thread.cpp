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

static void dumpStack(Frame& frame)
{
	Log::infof("Stack dump (length %d):", frame.stack->count());
	for (size_t i = 0; i < frame.stackIndex; i++)
	{
		Operand& value = frame.stack->get(i);
		if (value.isObject)
		{
			Log::infof("[%d] %x (obj)", i, value.object);
			value.object->validate();
		}
		else
			Log::infof("[%d] %d", i, value.integer);
	}
}

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
		Log::infof("Executing opcode %d (%s) (index: %d)", instruction.opcode, toString(instruction.opcode), pc);
		switch (instruction.opcode)
		{
		case Opcode::panic:
			Log::critical("Reached panic instruction");
			Arch::panic();
			break;
		case Opcode::dup:
			opcodeDup(frame);
			pc++;
			break;
		case Opcode::swap:
			opcodeSwap(frame);
			pc++;
			break;
		case Opcode::load_constant:
			opcodeLoadConstant(frame, instruction);
			storeInstruction = true;
			break;
		case Opcode::iconst:
			opcodeIconst(frame, instruction.constantInteger);
			pc++;
			break;
		case Opcode::load:
			opcodeLoad(frame, instruction.index);
			pc++;
			break;
		case Opcode::store:
			opcodeStore(frame, instruction.index);
			pc++;
			break;
		case Opcode::iadd:
			opcodeIadd(frame);
			pc++;
			break;
		case Opcode::imul:
			opcodeImul(frame);
			pc++;
			break;
		case Opcode::iinc:
			opcodeIinc(frame, instruction.varIncrement.variable, instruction.varIncrement.constant);
			pc++;
			break;
		case Opcode::i2b:
			opcodeI2B(frame);
			pc++;
			break;
		case Opcode::new_a:
			instruction = opcodeNewA(thread, frame, instruction.index);
			storeInstruction = true;
			break;
		case Opcode::new_b:
			instruction = opcodeNewB(frame);
			storeInstruction = true;
			break;
		case Opcode::new_c:
			opcodeNewC(frame, instruction.targetClass);
			pc++;
			break;
		case Opcode::newarray:
			opcodeNewArray(frame, static_cast<ArrayType>(instruction.index));
			pc++;
			break;
		case Opcode::arraylength:
			opcodeArrayLength(frame);
			pc++;
			break;
		case Opcode::goto_a:
			instruction.opcode = Opcode::goto_b;
			instruction.index = findOpcodeJumpTarget(frame, instruction.index);
			storeInstruction = true;
			break;
		case Opcode::goto_b:
			pc = instruction.index;
			break;
		case Opcode::if_icmpne_a:
			instruction.opcode = Opcode::if_icmpne_b;
			instruction.index = findOpcodeJumpTarget(frame, instruction.index);
			storeInstruction = true;
			break;
		case Opcode::if_icmpne_b:
			pc = opcodeIfIcmpneB(frame, instruction.index, pc);
			break;

		case Opcode::getstatic_a:
			findOpcodeFieldA(thread, frame, instruction.index);
			instruction.opcode = Opcode::getstatic_b;
			storeInstruction = true;
			break;
		case Opcode::getstatic_b:
			findOpcodeFieldB(frame, instruction.index, instruction);
			instruction.opcode = Opcode::getstatic_c;
			storeInstruction = true;
			break;
		case Opcode::getstatic_c:
			opcodeGetstaticC(frame, instruction.targetField);
			pc++;
			break;

		case Opcode::putstatic_a:
			findOpcodeFieldA(thread, frame, instruction.index);
			instruction.opcode = Opcode::putstatic_b;
			storeInstruction = true;
			break;
		case Opcode::putstatic_b:
			findOpcodeFieldB(frame, instruction.index, instruction);
			instruction.opcode = Opcode::putstatic_c;
			storeInstruction = true;
			break;
		case Opcode::putstatic_c:
			opcodePutstaticC(frame, instruction.targetField);
			pc++;
			break;

		case Opcode::getfield_a:
			findOpcodeFieldA(thread, frame, instruction.index);
			instruction.opcode = Opcode::getfield_b;
			storeInstruction = true;
			break;
		case Opcode::getfield_b:
			findOpcodeFieldB(frame, instruction.index, instruction);
			instruction.opcode = Opcode::getfield_c; //nextFieldOpcode(instruction, FieldOpcodeType::GET);
			storeInstruction = true;
			break;
		case Opcode::getfield_c:
			opcodeGetfieldC(frame, instruction.targetField);
			pc++;
			break;

		case Opcode::putfield_a:
			findOpcodeFieldA(thread, frame, instruction.index);
			instruction.opcode = Opcode::putfield_b;
			storeInstruction = true;
			break;
		case Opcode::putfield_b:
			findOpcodeFieldB(frame, instruction.index, instruction);
			instruction.opcode = Opcode::putfield_c;
			storeInstruction = true;
			break;
		case Opcode::putfield_c:
			opcodePutfieldC(frame, instruction.targetField);
			pc++;
			break;

		case Opcode::invoke_a:
			opcodeInvokeA(thread, frame, instruction.index);
			instruction.opcode = Opcode::invoke_b;
			storeInstruction = true;
			break;
		case Opcode::invoke_b:
			opcodeInvokeB(frame, instruction.index, instruction);
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
		case Opcode::invokevirtual_a:
			opcodeInvokevirtualA(thread, frame, instruction.index);
			instruction.opcode = Opcode::invokevirtual_b;
			storeInstruction = true;
			break;
		case Opcode::invokevirtual_b:
			opcodeInvokevirtualB(frame, instruction);
			instruction.opcode = Opcode::invokevirtual_c;
			storeInstruction = true;
			break;
		case Opcode::invokevirtual_c:
			opcodeInvokevirtualC(thread, frame, instruction.virtualArg.vtableIndex, instruction.virtualArg.stackOffset);
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
		dumpStack(frame.get());
		if (storeInstruction)
		{
			frame.get().instructions->get(pc) = instruction;
		}
		if (returnFromFrame)
		{
			if (thread.get().top == nullptr)
				return ThreadState::STOPPED;
			m_gc.makeRoot(thread.get().top, frame);
			pc = frame.get().pc;
		}
	}
	while (!m_interrupted);
	frame.get().pc = pc;
	return ThreadState::RUNNING;
}

void VM::opcodeDup(GC::Root<Frame>& frame)
{
	Operand value = frame.get().pop();
	frame.get().push(value);
	frame.get().push(value);
}

void VM::opcodeSwap(GC::Root<Frame>& frame)
{
	Operand a = frame.get().pop();
	Operand b = frame.get().pop();
	frame.get().push(a);
	frame.get().push(b);
}

void VM::opcodeLoadConstant(GC::Root<Frame>& frame, Instruction& instruction)
{
	ConstantPoolInfo& constant = frame.get().methodInfo->object.classFile->object.constantPool->get(instruction.index);
	switch (constant.tag)
	{
	case ConstantPoolTag::CONSTANT_integer:
		instruction.opcode = Opcode::iconst;
		instruction.constantInteger = constant.c_integer.integer;
		break;
	default:
		Log::critical("Unsupported or invalid constant pool type");
		Arch::panic();
		break;
	}
}

void VM::opcodeIconst(GC::Root<Frame>& frame, i32 value)
{
	Operand operand;
	operand.integer = value;
	frame.get().push(operand);
}

void VM::opcodeLoad(GC::Root<Frame>& frame, u16 index)
{
	Operand operand = frame.get().locals->get(index);
	frame.get().push(operand);
}

void VM::opcodeStore(GC::Root<Frame>& frame, u16 index)
{
	Operand operand = frame.get().pop();
	frame.get().locals->get(index) = operand;
}

void VM::opcodeIadd(GC::Root<Frame>& frame)
{
	Operand a = frame.get().pop();
	Operand b = frame.get().pop();
	Operand c;
	c.integer = a.integer + b.integer;
	frame.get().push(c);
}

void VM::opcodeImul(GC::Root<Frame>& frame)
{
	Operand a = frame.get().pop();
	Operand b = frame.get().pop();
	Operand c;
	c.integer = a.integer * b.integer;
	frame.get().push(c);
}

void VM::opcodeI2B(GC::Root<Frame>& frame)
{
	Operand operand = frame.get().pop();
	operand.integer = static_cast<i8>(operand.integer);
	frame.get().push(operand);
}

void VM::opcodeIinc(GC::Root<Frame>& frame, u8 variable, i32 amount)
{
	frame.get().locals->get(variable).integer += amount;
}

Instruction VM::opcodeNewA(GC::Root<Thread>& thread, GC::Root<Frame>& frame, u16 index)
{
	GC::Array<ConstantPoolInfo>* pool = frame.get().methodInfo->object.classFile->object.constantPool;
	i16 nameIndex = pool->get(index).c_class.nameIndex;

	GC::Root<char> name;
	m_gc.makeRoot(pool->get(nameIndex).c_utf8.bytes, name);

	findClass(thread, frame, name);

	Instruction result;
	result.opcode = Opcode::new_b;
	return result;
}

Instruction VM::opcodeNewB(GC::Root<Frame>& frame)
{
	Operand operand = frame.get().pop();

	Instruction result;
	result.opcode = Opcode::new_c;
	result.targetClass = operand.classFile;
	return result;
}

void VM::opcodeNewC(GC::Root<Frame>& frame, GC::Object<ClassFile>* classFile)
{
	GC::Root<ClassFile> root;
	m_gc.makeRoot(classFile, root);

	GC::Root<JavaObject> object;
	allocateObject(root, object);

	Operand result;
	result.isObject = true;
	object.store(&result.object);

	frame.get().push(result);
}

void VM::opcodeNewArray(GC::Root<Frame>& frame, ArrayType arrayType)
{
	GC::Allocator<JavaArray> arrayAllocator(JavaArray::describer);

	size_t bytesPerValue;
	switch (arrayType)
	{
	case ArrayType::BOOLEAN:
	case ArrayType::BYTE:
	case ArrayType::CHAR:
		bytesPerValue = 1;
		break;
	case ArrayType::SHORT:
		bytesPerValue = 2;
		break;
	case ArrayType::INT:
	case ArrayType::FLOAT:
		bytesPerValue = 4;
		break;
	case ArrayType::LONG:
	case ArrayType::DOUBLE:
	case ArrayType::REFERENCE:
		Log::critical("Unsupported array type");
		Arch::panic();
		break;
	default:
		Log::critical("Invalid array type");
		Arch::panic();
		break;
	}

	i32 length = frame.get().pop().integer;

	arrayAllocator.size = length * bytesPerValue;
	GC::Root<JavaArray> array;
	m_gc.allocate(arrayAllocator, array);

	array.get().type = arrayType;
	array.get().length = length;

	Operand operand;
	operand.isObject = true;
	array.store(&operand.array);
	frame.get().push(operand);
}

void VM::opcodeArrayLength(GC::Root<Frame>& frame)
{
	Operand operand = frame.get().pop();
	Operand result;
	result.integer = operand.array->object.length;
	frame.get().push(result);
}

u16 VM::opcodeIfIcmpneB(GC::Root<Frame>& frame, u16 target, u16 pc)
{
	Operand a = frame.get().pop();
	Operand b = frame.get().pop();
	if (a.integer != b.integer)
		return target;
	else
		return pc + 1;
}

void VM::opcodeGetstaticC(GC::Root<Frame>& frame, GC::Object<FieldInfo>* field)
{
	Operand& operand = field->object.value;
	frame.get().push(operand);
}

void VM::opcodePutstaticC(GC::Root<Frame>& frame, GC::Object<FieldInfo>* field)
{
	Operand operand = frame.get().pop();
	field->object.value = operand;
}

void VM::opcodeGetfieldC(GC::Root<Frame>& frame, GC::Object<FieldInfo>* field)
{
	Operand operand = frame.get().pop();
	operand.object->validate();
	void* value = operand.object->object.getFieldAt<void>(field->object.offset);
	Operand result;
	switch (field->object.type.type)
	{
	case Type::BOOLEAN:
	case Type::BYTE:
	case Type::CHAR:
		result.integer = *static_cast<i8*>(value);
		break;
	case Type::SHORT:
		result.integer = *static_cast<i16*>(value);
		break;
	case Type::INTEGER:
	case Type::FLOAT:
		result.integer = *static_cast<i32*>(value);
		break;
	case Type::REFERENCE:
		result.object = *static_cast<GC::Object<JavaObject>**>(value);
		break;
	case Type::VOID:
	case Type::LONG:
	case Type::DOUBLE:
		Arch::panic();
		break;
	}
	frame.get().push(result);
}

void VM::opcodePutfieldC(GC::Root<Frame>& frame, GC::Object<FieldInfo>* field)
{
	field->validate();
	Operand value = frame.get().pop();
	Operand object = frame.get().pop();
	object.object->validate();
	void* target = object.object->object.getFieldAt<void>(field->object.offset);
	switch (field->object.type.type)
	{
	case Type::BOOLEAN:
	case Type::BYTE:
	case Type::CHAR:
		*static_cast<i8*>(target) = value.integer;
		break;
	case Type::SHORT:
		*static_cast<i16*>(target) = value.integer;
		break;
	case Type::INTEGER:
	case Type::FLOAT:
		*static_cast<i32*>(target) = value.integer;
		break;
	case Type::REFERENCE:
		*static_cast<GC::Object<JavaObject>**>(target) = value.object;
		break;
	case Type::VOID:
	case Type::LONG:
	case Type::DOUBLE:
		Arch::panic();
		break;
	}
}

void VM::opcodeInvokeA(GC::Root<Thread>& thread, GC::Root<Frame>& frame, u16 index)
{
	ClassFile& classFile = frame.get().getClassFile()->object;
	ConstantPoolMethodRef methodRef = classFile.constantPool->get(index).c_method;

	GC::Root<char> className;
	u16 classNameIndex = classFile.constantPool->get(methodRef.classIndex).c_class.nameIndex;
	m_gc.makeRoot(classFile.constantPool->get(classNameIndex).c_utf8.bytes, className);

	findClass(thread, frame, className);
}

void VM::opcodeInvokeB(GC::Root<Frame>& frame, u16 index, Instruction& instruction)
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

	if (method->object.isNative())
	{
		instruction.opcode = Opcode::invoke_native;
		instruction.targetNativeMethod = nullptr;

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
			instruction.targetNativeMethod = nativeMethod.method;
		}
		if (instruction.targetNativeMethod == nullptr)
		{
			Log::criticalf("Unknown native method");
			Arch::panic();
		}
	}
	else
	{
		instruction.opcode = Opcode::invoke_special;
		instruction.targetMethod = method;
	}
}

void VM::opcodeInvokeSpecial(GC::Root<Thread>& thread, GC::Root<Frame>& frame, GC::Object<MethodInfo>* method)
{
	GC::Root<MethodInfo> targetMethod;
	m_gc.makeRoot(method, targetMethod);
	invokeMethod(thread, targetMethod, frame.get().inInterrupt);
}

void VM::opcodeInvokevirtualA(GC::Root<Thread>& thread, GC::Root<Frame>& frame, u16 index)
{
	ClassFile& classfile = frame.get().getClassFile()->object;
	ConstantPoolMethodRef constantMethodRef = classfile.constantPool->get(index).c_method;
	ConstantPoolClass constantClass = classfile.constantPool->get(constantMethodRef.classIndex).c_class;
	GC::Root<char> className;
	m_gc.makeRoot(classfile.constantPool->get(constantClass.nameIndex).c_utf8.bytes, className);
	findClass(thread, frame, className);
}

void VM::opcodeInvokevirtualB(GC::Root<Frame>& frame, Instruction& instruction)
{
	ClassFile& classfile = frame.get().getClassFile()->object;
	ConstantPoolMethodRef methodRef = classfile.constantPool->get(instruction.index).c_method;

	GC::Root<char> methodName;
	u16 methodNameIndex = classfile.constantPool->get(methodRef.nameAndTypeIndex).c_nameAndType.nameIndex;
	m_gc.makeRoot(classfile.constantPool->get(methodNameIndex).c_utf8.bytes, methodName);

	GC::Root<char> methodType;
	u16 methodTypeIndex = classfile.constantPool->get(methodRef.nameAndTypeIndex).c_nameAndType.descriptorIndex;
	m_gc.makeRoot(classfile.constantPool->get(methodTypeIndex).c_utf8.bytes, methodType);

	Operand operand = frame.get().pop();
	GC::Object<ClassFile>* targetClass = operand.classFile;

	u16 methodIndexInTargetClass = targetClass->object.findMethodByNameAndType(methodName, methodType);
	MethodInfo& method = targetClass->object.methods->get(methodIndexInTargetClass).method->object;
	u16 vtableIndex = method.vtableIndex;

	instruction.virtualArg.vtableIndex = vtableIndex;
	instruction.virtualArg.stackOffset = method.type.arguments;
}

void VM::opcodeInvokevirtualC(GC::Root<Thread>& thread, GC::Root<Frame>& frame, u16 index, u16 objectOffset)
{
	Operand operand = frame.get().peek(objectOffset);
	GC::Object<MethodInfo>* methodPtr = operand.object->object.class_->object.vtable->get(index).method;
	GC::Root<MethodInfo> method;
	m_gc.makeRoot(methodPtr, method);
	invokeMethod(thread, method, frame.get().inInterrupt);
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

void VM::findOpcodeFieldA(GC::Root<Thread>& thread, GC::Root<Frame>& frame, u16 index)
{
	ClassFile& classFile = frame.get().getClassFile()->object;
	ConstantPoolFieldRef fieldRef = classFile.constantPool->get(index).c_field;

	GC::Root<char> className;
	u16 classNameIndex = classFile.constantPool->get(fieldRef.classIndex).c_class.nameIndex;
	m_gc.makeRoot(classFile.constantPool->get(classNameIndex).c_utf8.bytes, className);

	findClass(thread, frame, className);
}

void VM::findOpcodeFieldB(GC::Root<Frame>& frame, u16 index, Instruction& instruction)
{
	ClassFile& classFile = frame.get().getClassFile()->object;
	ConstantPoolFieldRef fieldRef = classFile.constantPool->get(index).c_field;

	GC::Root<char> fieldName;
	u16 fieldNameIndex = classFile.constantPool->get(fieldRef.nameAndTypeIndex).c_nameAndType.nameIndex;
	m_gc.makeRoot(classFile.constantPool->get(fieldNameIndex).c_utf8.bytes, fieldName);

	Operand operand = frame.get().pop();
	GC::Object<ClassFile>* targetClass = operand.classFile;

	u16 fieldIndex = targetClass->object.findFieldByName(fieldName);

	instruction.targetField = targetClass->object.fields->get(fieldIndex).field;
}

u16 VM::findOpcodeJumpTarget(GC::Root<Frame>& frame, u16 target)
{
	GC::Array<Instruction>* instructions = frame.get().instructions;
	for (size_t i = 0; i < instructions->count(); i++)
	{
		Instruction instruction = instructions->get(i);
		if (instruction.offset == target)
		{
			return i;
		}
	}
	Log::critical("Found no target instruction for jump");
	Arch::panic();
	return 0;
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
		operand.isObject = true;
		targetClass.store(&operand.classFile);
		frame.get().push(operand);
	}
	else
	{
		Log::critical("Non-native classloader not yet supported");
		Arch::panic();
	}
}

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


























































