/**@file
 * @brief
 */
#ifndef _JVM_THREAD_HPP_
#define _JVM_THREAD_HPP_

//#include "jvm_method.hpp"
#include "gc.hpp"
#include "jvm_defs.hpp"

namespace Java
{
	struct Instruction;
	class VM;

	struct Frame
	{
		GC::Object<MethodInfo>* methodInfo;
		GC::Object<Frame>* previous = nullptr;
		GC::Array<Operand>* locals;
		GC::Array<Operand>* stack;
//		GC::Object<CodeAttribute>* code;
		GC::Array<Instruction>* instructions;
		u16 pc;
		u16 stackIndex;
		bool inInterrupt;

		Instruction& getInstruction(size_t index);

//		u8 getU8FromCode(size_t index);
//		u16 getU16FromCode(size_t index);

		GC::Object<ClassFile>* getClassFile();

		/**
		 * Pushes an item onto the stack.
		 *
		 * @param operand The item to push.
		 */
		void push(Operand& operand);

		/**
		 * Pops an item from the stack.
		 *
		 * @return The item that was popped.
		 */
		Operand pop();

		/**
		 * Peeks at an item at a certain offset from the top of the stack.
		 *
		 * @param offset The offset from the top of the stack.
		 *
		 * @return The item at that location.
		 */
		Operand& peek(u16 offset);

		static void describer(GC::Meta* object, GC::MetaVisitor& visitor);
	};

	struct Thread
	{
		GC::Object<Frame>* top = nullptr;

		static void describer(GC::Meta* object, GC::MetaVisitor& visitor);
	};

	struct VarIncrement
	{
		u8 variable;
		i8 constant;
	};

	struct InstructionVirtualArg
	{
		u16 vtableIndex;
		u16 stackOffset;
	};

	struct Instruction
	{
		Opcode opcode;
		u16 offset;

		static void describer(GC::Meta* object, GC::MetaVisitor& visitor);
		union
		{
			u16 index;
			struct
			{
				u16 utf8Index;
				u16 stringIndex;
			} protoString;
			InstructionVirtualArg virtualArg;
			i32 constantInteger;
			VarIncrement varIncrement;
			GC::Object<ClassFile>* targetClass;
			GC::Object<MethodInfo>* targetMethod;
			GC::Object<FieldInfo>* targetField;
			GC::Object<JavaObject>* targetObject;
			void (*targetNativeMethod)(VM& vm, GC::Root<Thread>& thread);
		};
	};
}


#endif /* _JVM_THREAD_HPP_ */
