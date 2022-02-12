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
		void (*method)(VM& vm, GC::Root<Thread>& thread);
	};

	/**
	 * A class-loader to be implemented in C/C++.
	 */
	struct NativeClassLoader
	{
		ClassError (*loadClass)(void*, VM& vm, GC::Root<Java::Thread>& thread, GC::Root<ClassFile>& root, const GC::Root<char>& name);
	};

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
		 */
		void createThread(GC::Root<Thread>& thread);

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
		 * Invokes a method on a thread.
		 *
		 * @param thread The thread to add a frame on.
		 * @param classfile The classfile containing the method.
		 * @param method The method for whom to add the frame for.
		 * @param isInterrupt Set to `true` if the method call should be executed
		 * before any other method calls, even before methods that are invoked
		 * after this one (unless those too are interrupt calls).
		 */
		void invokeMethod(GC::Root<Thread>& thread, const GC::Root<ClassFile>& targetClass, u16 method, bool isInterrupt = false);

		/**
		 * Invokes a method on a thread.
		 *
		 * @param thread The thread to add a frame on.
		 * @param classfile The classfile containing the method.
		 * @param method The method for whom to add the frame for.
		 * @param isInterrupt Set to `true` if the method call should be executed
		 * before any other method calls, even before methods that are invoked
		 * after this one (unless those too are interrupt calls).
		 */
		void invokeMethod(GC::Root<Thread>& thread, const GC::Root<ClassFile>& classfile, const char* method);
		void invokeMethod(GC::Root<Thread>& thread, GC::Root<MethodInfo>& targetMethod, bool isInterrupt = false);

		/**
		 * Performs a single step in a thread.
		 *
		 * @param thread The thread to step through.
		 */
		ThreadState step(GC::Root<Thread>& thread);
		ThreadState run(GC::Root<Thread>& thread);

		/**
		 * Gets a root to a class by the FQN of the class.
		 *
		 * If needed, the class will be loaded into the VM.
		 *
		 * @param classfile The loaded class file.
		 * @param thread The thread that is loading the class.
		 * @param name The name of the class.
		 *
		 * @return An error code if the class could not be loaded.
		 */
		[[nodiscard]]
		ClassError getClass(GC::Root<ClassFile>& classfile, GC::Root<Thread>& thread, const GC::Root<char>& name);

		/**
		 * Finds a class using a normal JVM class-loading scheme.
		 *
		 * If the calling thread was loaded using a native class loader, it will
		 * pass the call on to @ref getClass. A reference to the class will be
		 * pushed onto the operand stack.
		 *
		 * If the calling thread was loaded using a Java class loader, the class
		 * loader will be invoked as a normal method. It will push the result
		 * onto the operand stack as a normal return value.
		 *
		 * @param thread The thread that will search for a class.
		 * @param frame The frame that performed the call.
		 * @param name The name of the class to search for.
		 */
		void findClass(GC::Root<Thread>& thread, GC::Root<Frame>& frame, const GC::Root<char>& name);

		/**
		 * Loads a class into the VM.
		 *
		 * Note that it does not create any references to the class within the
		 * VM, so a garbage collection cycle will remove the class unless you
		 * do something with its root.
		 *
		 * @param classfile A root to the loaded class file.
		 * @param thread The thread that is creating the class file.
		 * @param data The class file.
		 * @param length The number of bytes in the file.
		 *
		 * @return An error code if the class could not be loaded.
		 */
		[[nodiscard]]
		ClassError defineClass(GC::Root<ClassFile>& classfile, GC::Root<Thread>& thread, const u8* data, size_t length);

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

		/// Set to true to interrupt.
		bool m_interrupted = false;

		ThreadState runUntilInterrupted(GC::Root<Thread>& thread);

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

		void parseOpcodes(GC::Root<Instruction>& instructions, Loader& loader, size_t instructionCount);

		void createFrame(GC::Root<Frame>& frame, GC::Root<MethodInfo>& method);

//		void returnFromMethod(GC::Root<Thread>& thread);

		/**
		 * Pops a frame and pushed the item on the top of the current stack to
		 * the new stack.
		 *
		 * @param thread The thread to pop.
		 */
//		void returnInteger(GC::Root<Thread>& thread);

//		void pushIntegerFromVariable(Frame& frame, i32 number);
//		void pushReferenceFromVariable(Frame& frame, i32 number);
//		void storeIntegerToVariable(Frame& frame, i32 number);
//		void storeReferenceToVariable(Frame& frame, i32 number);
//		void pushInteger(Frame& frame, i32 number);
//		void pushShort(Frame& frame);
//		void pushByte(Frame& frame);
//		void pushConstant(Frame& frame);
//		void dup(Frame& frame);
//		void swap(Frame& frame);
//		void addIntegers(Frame& frame);
//		void increment(Frame& frame);
		u16 findOpcodeJumpTarget(GC::Root<Frame>& frame, u16 target);
		void findOpcodeFieldA(GC::Root<Thread>& thread, GC::Root<Frame>& frame, u16 index);
		void findOpcodeFieldB(GC::Root<Frame>& frame, u16 index, Instruction& instruction);
		void opcodeIconst(GC::Root<Frame>& frame, i32 value);
		void opcodeAload(GC::Root<Frame>& frame, u16 index);
		void opcodeIload(GC::Root<Frame>& frame, u16 index);
		u16 opcodeIfIcmpneB(GC::Root<Frame>& frame, u16 target, u16 pc);

//		Instruction opcodeFindFieldA;
//		Instruction opcodeGetfieldB(GC::Root<Frame>& frame, u16 index);
		void opcodeGetfieldC(GC::Root<Frame>& frame, GC::Object<FieldInfo>* field);

//		Instruction opcodePutfieldA(GC::Root<Thread>& thread, GC::Root<Frame>& frame, u16 index);
//		Instruction opcodePutfieldB(GC::Root<Frame>& frame, u16 index);
		void opcodePutfieldC(GC::Root<Frame>& frame, GC::Object<FieldInfo>* field);

		void opcodeInvokeA(GC::Root<Thread>& thread, GC::Root<Frame>& frame, u16 index);
		void opcodeInvokeB(GC::Root<Frame>& frame, u16 index, Instruction& instruction);
		void opcodeInvokeSpecial(GC::Root<Thread>& thread, GC::Root<Frame>& frame, GC::Object<MethodInfo>* method);
		void opcodeReturnValue(GC::Root<Thread>& thread, GC::Root<Frame>& frame);
		void opcodeReturn(GC::Root<Thread>& thread, GC::Root<Frame>& frame);
		void invokeStatic(GC::Root<Thread>& thread);
		void invokeSpecial(GC::Root<Thread>& thread);
		void invokeNativeMethod(GC::Root<Thread>& thread, const GC::Root<char>& className, const GC::Root<char>& methodName, const GC::Root<char>& methodType);
//		void jumpIfIntegerNotEqual(Frame& frame);
//		void jumpIfIntegerLessThan(Frame& frame);
//		void jumpUnconditionally(Frame& frame);
//		void getStatic(GC::Root<Thread>& thread, GC::Root<Frame>& frame);
//		void putStatic(GC::Root<Thread>& thread, GC::Root<Frame>& frame);
//		void getField(GC::Root<Thread>& thread, GC::Root<Frame>& frame);
//		void putField(GC::Root<Thread>& thread, GC::Root<Frame>& frame);
//		Operand* findField(GC::Root<Thread>& thread, GC::Root<Frame>& frame);
//		void newObject(GC::Root<Thread>& thread, GC::Root<Frame>& frame);
	};
}

#endif /* _JVM_CONTEXT_HPP_ */
