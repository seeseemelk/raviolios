#include "cut.hpp"

#include "asserts.hpp"

#include <cstring>

using namespace Java;

static const char* s_validClasses[] = {
		"Test1",
		"StaticReturnsInt",
		"raviolios/Arch"
};
static size_t s_validClassesCount = sizeof(s_validClasses) / sizeof(const char*);

CUT::CUT(size_t memorySize)
	: vm(m_cachingClassLoader)
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

ClassError CUT::loadClass(GC::Root<Java::ClassFile>& classfile, std::string classname)
{
	Buffer buffer;
	loadTestClass(buffer, classname);
	Java::ClassError error = vm.defineClass(classfile, buffer.data, buffer.length);
	assertEquals(Java::ClassError::GOOD, error, "Class loaded without errors");
	return error;
}

ClassError CUT::loadClass(VM& /*vm*/, GC::Root<ClassFile>& root, const GC::Root<char>& name)
{
	for (size_t i = 0; i < s_validClassesCount; i++)
	{
		if (strcmp(name.asPtr(), s_validClasses[i]) == 0)
		{
			return loadClass(root, std::string(name.asPtr()));
		}
	}
	return ClassError::NOT_FOUND;
}
