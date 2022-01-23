#include "cut.hpp"

#include "asserts.hpp"
#include "util.hpp"

#include <iostream>

using namespace Java;

const char* s_object = "raviolios/java/lang/Object";

static const char* s_validClasses[] = {
		"Test1",
		"StaticReturnsInt",
		"raviolios/Arch",
		"java/lang/Object"
};
static size_t s_validClassesCount = sizeof(s_validClasses) / sizeof(const char*);

CUT::CUT(size_t memorySize)
	: CUT(nullptr, 0, memorySize)
{
}

CUT::CUT(Java::NativeMethod* nativeMethods, size_t nativeMethodCount, size_t memorySize)
{
	m_cachingClassLoader.parent(*this);
	vm.init(m_cachingClassLoader, nativeMethods, nativeMethodCount);
	memory = malloc(memorySize);
	vm.gc().init(memory, memorySize);
}


CUT::~CUT()
{
	vm.gc().deinit();
	free(memory);
}

ClassError CUT::loadClass(GC::Root<Java::ClassFile>& classfile, std::string className)
{
	GC::Root<char> classNameRoot;
	vm.allocateString(classNameRoot, className.c_str());
	return vm.getClass(classfile, classNameRoot);
}

ClassError CUT::loadClass(VM& /*vm*/, GC::Root<ClassFile>& classfile, const GC::Root<char>& name)
{
	for (size_t i = 0; i < s_validClassesCount; i++)
	{
		if (equals(name, s_validClasses[i]) == 0)
		{
			Buffer buffer;
			std::string nameStr(name.asPtr(), name.object->size);
			if (!loadTestClass(buffer, nameStr))
				return ClassError::NOT_FOUND;
			Java::ClassError error = vm.defineClass(classfile, buffer.data, buffer.length);
			assertEquals(Java::ClassError::GOOD, error, "Class loaded without errors");
			return error;
		}
	}
	return ClassError::NOT_FOUND;
}
