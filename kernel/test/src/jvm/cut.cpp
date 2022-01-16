#include "cut.hpp"

#include "asserts.hpp"
#include "util.hpp"

#include <iostream>

using namespace Java;

const char* s_object = "raviolios/java/lang/Object";
const char* s_noObject = "raviolios/java/lang/NoObject";

static const char* s_validClasses[] = {
		"Test1",
		"StaticReturnsInt",
		"raviolios/Arch",
		"java/lang/Object",
		s_noObject
};
static size_t s_validClassesCount = sizeof(s_validClasses) / sizeof(const char*);

CUT::CUT(size_t memorySize)
	: CUT(nullptr, 0, memorySize)
{
}

CUT::CUT(Java::NativeMethod* nativeMethods, size_t nativeMethodCount, size_t memorySize)
	: vm(m_cachingClassLoader, nativeMethods, nativeMethodCount)
	, m_cachingClassLoader(*this)
{
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
			if (nameStr == s_noObject)
			{
				return ClassError::GOOD;
			}
			else if (nameStr == "java/lang/Object")
			{
				if (!loadTestClass(buffer, s_object))
					return ClassError::NOT_FOUND;
				Java::ClassError error = vm.defineClass(classfile, buffer.data, buffer.length);
				assertEquals(Java::ClassError::GOOD, error, "Class loaded without errors");
				if (error != ClassError::GOOD)
					return error;
				GC::Root<char> newClassNameRoot;
				vm.allocateString(newClassNameRoot, "java/lang/Object");

				u16 superClass = classfile.get().superClass;
				u16 superClassNameIndex = classfile.get().constantPool->get(superClass).c_class.nameIndex;
				newClassNameRoot.store(&classfile.get().constantPool->get(superClassNameIndex).c_utf8.bytes);

				return ClassError::GOOD;
			}
			else
			{
				if (!loadTestClass(buffer, nameStr))
					return ClassError::NOT_FOUND;
				Java::ClassError error = vm.defineClass(classfile, buffer.data, buffer.length);
				assertEquals(Java::ClassError::GOOD, error, "Class loaded without errors");
				return error;
			}
		}
	}
	return ClassError::NOT_FOUND;
}
