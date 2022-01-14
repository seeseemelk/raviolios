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

class CUT : public Java::NativeClassLoader
{
public:
	Java::VM vm;
	void* memory;

	CUT(size_t memorySize = MB(1));
	~CUT();
	Java::ClassError loadClass(GC::Root<Java::ClassFile>& classfile, std::string classname);
	Java::ClassError loadClass(Java::VM& vm, GC::Root<Java::ClassFile>& root, const GC::Root<char>& name) override;

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
