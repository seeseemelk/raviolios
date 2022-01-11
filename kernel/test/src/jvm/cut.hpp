/**
 * @file
 * @brief
 */

#ifndef _CUT_HPP_
#define _CUT_HPP_

#include "jvm_context.hpp"
#include "fileloader.hpp"

#include <string>

struct CUT
{
	Java::VM vm;
	void* memory;

	CUT(size_t memorySize = MB(1))
	{
		memory = malloc(memorySize);
		vm.gc().init(memory, memorySize);
	}

	~CUT()
	{
		vm.gc().deinit();
		free(memory);
	}

	void loadClass(GC::Root<Java::ClassFile>& classfile, std::string classname)
	{
		Buffer buffer;
		loadTestClass(buffer, classname);
		Java::ClassError error = vm.defineClass(classfile, buffer.data, buffer.length);
		assertEquals(Java::ClassError::GOOD, error, "Class loaded without errors");
	}

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
};

#endif
