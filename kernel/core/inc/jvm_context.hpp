/**@file
 * @brief
 */
#ifndef _JVM_CONTEXT_HPP_
#define _JVM_CONTEXT_HPP_

#include "gc.hpp"
#include "jvm_defs.hpp"
#include "jvm_loader.hpp"
#include "jvm_thread.hpp"

namespace Java
{
	class VM;

	enum class ClassError
	{
		GOOD,
		NOT_FOUND,
		BAD_CONSTANT
	};

	enum class ThreadState
	{
		RUNNING,
		STOPPED
	};

	enum class ThreadCreateResult
	{
		CREATED,
		NO_METHOD
	};

	/**
	 * A class-loader to be implemented in C/C++.
	 */
	class NativeClassLoader
	{
	public:
		virtual ~NativeClassLoader() = default;

		/**
		 * Loads a class.
		 *
		 * The loader should call @ref VM::defineClass with the contents of the
		 * class file.
		 * If the class could not be found, the root should not be set.
		 *
		 * @param vm The VM to load the class into.
		 * @param root The GC root the class file should be loaded into.
		 * @param name The name of the class to load.
		 *
		 * @return The return value of @ref VM::defineClass, or @ref ClassError::NOT_FOUND
		 * if the class could not be found.
		 */
		virtual ClassError loadClass(VM& vm, GC::Root<ClassFile>& root, const GC::Root<char>& name) = 0;
	};

	/**
	 * A thing that can load and run Java code.
	 */
	class VM
	{
	public:
		VM(NativeClassLoader& loader);

		/**
		 * Get the garbage collector of the VM.
		 *
		 * @return The garbage collector used by the VM.
		 */
		GC::Context& gc();

		/**
		 * Allocates a string on the garbage collected heap.
		 *
		 * @param root A root to the allocated string.
		 * @param str The string to allocate.
		 */
		void allocateString(GC::Root<char>& root, const char* str);

		/**
		 * Creates a thread.
		 *
		 * @param thread The thread to create.
		 * @param classfile The classfile containing the method to start executing.
		 * @param method The index into the classfile where the method can be found.
		 */
		ThreadCreateResult createThread(GC::Root<Thread>& thread, const GC::Root<ClassFile>& classfile, u16 method);

		/**
		 * Creates a thread.
		 *
		 * @param thread The thread to create.
		 * @param classfile The classfile containing the method to start executing.
		 * @param method The name of the method.
		 */
		ThreadCreateResult createThread(GC::Root<Thread>& thread, const GC::Root<ClassFile>& classfile, const char* method);

		/**
		 * Performs a single step in a thread.
		 *
		 * @param thread The thread to step through.
		 */
		ThreadState step(GC::Root<Thread>& thread);

		/**
		 * Loads a class into the VM.
		 *
		 * @param classfile The loaded class file.
		 * @param name The name of the class.
		 *
		 * @return An error code if the class could not be loaded.
		 */
		ClassError loadClass(GC::Root<ClassFile>& classfile, const GC::Root<char> name);

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
		 *
		 * @return An error code if the class could not be loaded.
		 */
		ClassError defineClass(GC::Root<ClassFile>& classfile, const u8* data, size_t length);

	private:
		/// The native class loader.
		NativeClassLoader& m_classLoader;

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

		void createFrame(GC::Root<Frame>& frame, MethodInfo& method);

		/**
		 * Pops a frame and pushed the item on the top of the current stack to
		 * the new stack.
		 *
		 * @param thread The thread to pop.
		 */
		void returnInteger(GC::Root<Thread>& thread);

		void pushInteger(Frame& frame, i32 number);
	};
}

#endif /* _JVM_CONTEXT_HPP_ */
