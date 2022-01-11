/**@file
 * @brief
 */
#ifndef _JVM_CONTEXT_HPP_
#define _JVM_CONTEXT_HPP_

#include "gc.hpp"
#include "jvm_defs.hpp"
#include "jvm_loader.hpp"

namespace Java
{
	enum class ClassError
	{
		GOOD,
		BAD_CONSTANT
	};

	/**
	 * A thing that can load and run Java code.
	 */
	class VM
	{
	public:
		/**
		 * Get the garbage collector of the VM.
		 *
		 * @return The garbage collector used by the VM.
		 */
		GC::Context& gc();

		/**
		 * Loads a class into the VM.
		 *
		 * Note that it does not create any references to the class within the
		 * VM, so a garbage collection cycle will remove the class unless you
		 * do something with its root.
		 *
		 * @param classfile A root to the loaded class file.
		 * @param data The class file.
		 * @param length The number of bytes in the file.
		 */
		ClassError defineClass(GC::Root<Java::ClassFile>& classfile, const u8* data, size_t length);

	private:
		/// The GC context
		GC::Context m_gc;

		/**
		 * Allocates an array on the heap.
		 *
		 * @param length The length of the array.
		 */
		void allocateArray(GC::Root<u8>& root, size_t length);

		/**
		 * Allocates an array on the heap.
		 *
		 * @param length The length of the array.
		 */
		void allocateArray(GC::Root<char>& root, size_t length);

		/**
		 * Loads a list of attributes.
		 *
		 * @param root The root to store the attributes into.
		 * @param count The number of attributes to load.
		 */
		void loadAttributes(GC::Root<ClassFile>& classfile, GC::Root<AttributeInfo>& root, Loader& loader, size_t count);

		void loadCodeAttribute(GC::Root<ClassFile>& classfile, GC::Root<CodeAttribute>& root, Loader& loader);
	};
}

#endif /* _JVM_CONTEXT_HPP_ */
