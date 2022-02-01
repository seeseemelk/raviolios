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
		BAD_CONSTANT,
	};

	const char* toString(ClassError error);

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

	const char* toString(ThreadCreateResult error);

	struct NativeMethod
	{
		const char* className;
		const char* methodName;
		const char* methodType;
		void (*method)();
	};

	/**
	 * A class-loader to be implemented in C/C++.
	 */
	struct NativeClassLoader
	{
		ClassError (*loadClass)(void*, VM& vm, GC::Root<ClassFile>& root, const GC::Root<char>& name);
	};
//	class NativeClassLoader
//	{
//	public:
//		/**
//		 * Loads a class.
//		 *
//		 * The loader should call @ref VM::defineClass with the contents of the
//		 * class file.
//		 * If the class could not be found, the root should not be set.
//		 *
//		 * @param vm The VM to load the class into.
//		 * @param root The GC root the class file should be loaded into.
//		 * @param name The name of the class to load.
//		 *
//		 * @return The return value of @ref VM::defineClass, or @ref ClassError::NOT_FOUND
//		 * if the class could not be found.
//		 */
//		[[nodiscard]]
//		virtual ClassError loadClass(VM& vm, GC::Root<ClassFile>& root, const GC::Root<char>& name) = 0;
//	};

	/**
	 * A thing that can load and run Java code.
	 */
	class VM
	{
	public:
		void init(const NativeClassLoader& loaderVtable, void* loader, const NativeMethod* nativeMethods, size_t nativeMethodCount);

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
		 * Gets a root to a class by the FQN of the class.
		 *
		 * If needed, the class will be loaded into the VM.
		 *
		 * @param classfile The loaded class file.
		 * @param name The name of the class.
		 *
		 * @return An error code if the class could not be loaded.
		 */
		[[nodiscard]]
		ClassError getClass(GC::Root<ClassFile>& classfile, const GC::Root<char>& name);

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
		[[nodiscard]]
		ClassError defineClass(GC::Root<ClassFile>& classfile, const u8* data, size_t length);

	private:
		/// The native class loader.
		const NativeClassLoader* m_classLoaderVtable;
		void* m_classLoader;

		/// The GC context
		GC::Context m_gc;

		/// The number of registered native methods.
		size_t m_nativeMethodCount = 0;

		/// An array of native methods.
		const NativeMethod* m_nativeMethods;

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

		void createFrame(GC::Root<Frame>& frame, GC::Root<MethodInfo>& method);

		void returnFromMethod(GC::Root<Thread>& thread);

		/**
		 * Pops a frame and pushed the item on the top of the current stack to
		 * the new stack.
		 *
		 * @param thread The thread to pop.
		 */
		void returnInteger(GC::Root<Thread>& thread);

		void pushIntegerFromVariable(Frame& frame, i32 number);
		void pushInteger(Frame& frame, i32 number);
		void pushShort(Frame& frame);
		void pushByte(Frame& frame);
		void invokeStatic(GC::Root<Thread>& thread);
		void invokeNativeMethod(const GC::Root<char>& className, const GC::Root<char>& methodName, const GC::Root<char>& methodType);
		void jumpIfIntegerNotEqual(Frame& frame);
		void jumpIfIntegerLessThan(Frame& frame);
		void jumpUnconditionally(Frame& frame);
		void getStatic(GC::Root<Frame>& frame);
		void putStatic(GC::Root<Frame>& frame);
	};
}

#endif /* _JVM_CONTEXT_HPP_ */
