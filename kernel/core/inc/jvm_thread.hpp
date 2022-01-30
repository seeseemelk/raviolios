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
	struct Operand
	{
		union
		{
			i32 integer;
		};

		static void describer(GC::Meta* object, GC::MetaVisitor& visitor);
	};

	struct Frame
	{
		GC::Object<MethodInfo>* methodInfo;
		GC::Object<Frame>* previous = nullptr;
		GC::Array<Operand>* locals;
		GC::Array<Operand>* stack;
		GC::Object<CodeAttribute>* code;
		u16 pc;
		u16 stackIndex;

		Opcode getOpcode(size_t index);

		u8 getU8FromCode(size_t index);
		u16 getU16FromCode(size_t index);

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

		static void describer(GC::Meta* object, GC::MetaVisitor& visitor);
	};

	struct Thread
	{
		GC::Object<Frame>* top = nullptr;

		static void describer(GC::Meta* object, GC::MetaVisitor& visitor);
	};
}


#endif /* _JVM_THREAD_HPP_ */
