#include "asserts.hpp"

#include "cut.hpp"
#include "fileloader.hpp"
#include "jvm_context.hpp"

using namespace Java;

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
	// This can vary based on compiler version and flags
	//assertEquals(13, classfile.constantPoolCount, "Correct number of items in the constant pool");
	assertEquals(ACC_PUBLIC | ACC_SUPER, classfile.accessFlags, "Correct access flags");
	assertEquals(0, classfile.interfacesCount, "Correct number of interfaces");

	assertEquals(1, classfile.fieldsCount, "Correct number of fields");
	GC::Root<FieldInfo> fields;
	cut.makeRoot(classfile.fields, fields);

	assertEquals(ACC_PRIVATE, fields[0].accessFlags, "Correct field access flags");
	assertStringEqual("field", fields[0].name, "Correct field name");

	GC::Root<MethodRef> methods;
	cut.makeRoot(classfile.methods, methods);
	assertEquals(1U, methods.count(), "Correct number of methods");
}

TEST("Can load super classes automatically")
{
	CUT cut;

	GC::Root<ClassFile> child;
	GC::Root<ClassFile> parent;

	cut.loadClass(child, "Test1");
	cut.loadClass(parent, "java/lang/Object");

	assertNotNull(child.object, "Loaded child class");
	assertNotNull(parent.object, "Loaded parent class");

	GC::Object<ClassFile>* parentPtr = child.get().superClassObj;
	assertNotNull(parentPtr, "Parent was set");
	assertEquals(&parentPtr->object, parent.asPtr(), "Correctly set super class");
}
