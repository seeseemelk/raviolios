#include "asserts.hpp"

#include "fileloader.hpp"
#include "jvm_context.hpp"

using namespace Java;

struct CUT
{
	VM vm;
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

	void loadClass(GC::Root<ClassFile>& classfile, std::string classname)
	{
		Buffer buffer;
		loadTestClass(buffer, classname);
		ClassError error = vm.defineClass(classfile, buffer.data, buffer.length);
		assertEquals(ClassError::GOOD, error, "Class loaded without errors");
	}

	template<typename T>
	void makeRoot(GC::Ref<T>& ref, GC::Root<T>& root)
	{
		vm.gc().makeRoot(ref, root);
	}
};

TEST("Can create a JVM instance")
{
	CUT cut;
	assertTrue(cut.vm.gc().getFree() > 0, "Has some memory free");
}

TEST("Can load class file")
{
	CUT cut;

	GC::Root<ClassFile> root;
	cut.loadClass(root, "Test1");

	ClassFile& classfile = root.get();
	assertEquals(55U, classfile.majorVersion, "Correct major version");
	assertEquals(0U, classfile.minorVersion, "Correct minor version");
	assertEquals(13, classfile.constantPoolCount, "Correct number of items in the constant pool");
	assertEquals(ACC_PUBLIC | ACC_SUPER, classfile.accessFlags, "Correct access flags");
	assertEquals(0, classfile.interfacesCount, "Correct number of interfaces");

	assertEquals(1, classfile.fieldsCount, "Correct number of fields");
	GC::Root<FieldInfo> fields;
	cut.makeRoot(*classfile.fields, fields);

	assertEquals(ACC_PRIVATE, fields[0].accessFlags, "Correct field access flags");
	assertStringEqual("field", &fields[0].name->get(), "Correct field name");
}
