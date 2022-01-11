/**@file
 * @brief
 */
#ifndef _JVM_THREAD_HPP_
#define _JVM_THREAD_HPP_

//#include "jvm_method.hpp"
#include "gc.hpp"

namespace Java
{
	struct Frame
	{
		GC::Object<Frame>* previous = nullptr;

		static void describer(GC::Meta* object, GC::MetaVisitor& visitor);
	};

	struct Thread
	{
		GC::Object<Frame>* top = nullptr;

		static void describer(GC::Meta* object, GC::MetaVisitor& visitor);
	};
}


#endif /* _JVM_THREAD_HPP_ */
