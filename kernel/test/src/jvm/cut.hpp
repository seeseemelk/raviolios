/**
 * @file
 * @brief
 */

#ifndef _CUT_HPP_
#define _CUT_HPP_

#include "fileloader.hpp"
#include "jvm_classloader.hpp"
#include "jvm_context.hpp"

#include <string>

/**
 * Set to `true` to perform many garbage collection cycles during the test.
 * This can be used to try and diagnose memory issues. Should be set to `false`
 * after a test.
 */
extern bool g_aggresiveGarbageCollection;

class CUT
{
public:
	static const Java::NativeClassLoader vtable;
	Java::VM vm;
	void* memory;

	CUT(size_t memorySize = MB(1));
	CUT(Java::NativeMethod* nativeMethods, size_t nativeMethodCount, size_t memorySize = MB(1));
	~CUT();
	Java::ClassError loadClass(GC::Root<Java::ClassFile>& classfile, GC::Root<Java::Thread>& thread, std::string className);
	void createThread(GC::Root<Java::Thread>& thread);

	template<typename T>
	void makeRoot(GC::Array<T>* ref, GC::Root<T>& root)
	{
		vm.gc().makeRoot(ref, root);
	}

	template<typename T>
	void makeRoot(GC::Object<T>* ref, GC::Root<T>& root)
	{
		vm.gc().makeRoot(ref, root);
	}

private:
	Java::CachingClassLoader m_cachingClassLoader;
};

#endif
