#include "asserts.hpp"

#include "cut.hpp"
#include "jvm_class.hpp"

using namespace Java;

TEST("Can get name of class file")
{
	CUT cut;
	GC::Root<ClassFile> classfile;
	cut.loadClass(classfile, "Test1");
	Class cl(cut.vm, classfile);

	GC::Root<char> name;
	cl.name(name);
	assertStringEqual("Test1", name, "Name is correct");
}
