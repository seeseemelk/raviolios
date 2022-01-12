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
		static void describer(GC::Meta* object, GC::MetaVisitor& visitor);
	};

	struct Frame
	{
		GC::Object<Frame>* previous = nullptr;
		GC::Array<Operand>* locals;
		GC::Array<Operand>* stack;
		u16 pc;
		GC::Object<CodeAttribute>* code;

		static void describer(GC::Meta* object, GC::MetaVisitor& visitor);
	};

	struct Thread
	{
		GC::Object<Frame>* top = nullptr;

		static void describer(GC::Meta* object, GC::MetaVisitor& visitor);
	};
}


#endif /* _JVM_THREAD_HPP_ */
