/**
 * @file
 * @brief
 */

#ifndef _JVM_CLASS_HPP_
#define _JVM_CLASS_HPP_

#include "jvm_context.hpp"

namespace Java
{
	class Class
	{
	public:
		Class(VM& vm, GC::Object<ClassFile>* classfile);
		Class(VM& vm, GC::Root<ClassFile>& classfile);

		/**
		 * Gets the name of a class.
		 *
		 * @param name The name of the class.
		 */
		void name(GC::Root<char>& name);

	private:
		GC::Root<ClassFile> m_classfile;
		GC::Root<char> m_name;
		VM m_vm;
	};
}

#endif
