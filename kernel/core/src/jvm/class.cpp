#include "jvm_class.hpp"

using namespace Java;

Class::Class(VM& vm, GC::Object<ClassFile>* classfile)
	: m_vm(vm)
{
	vm.gc().makeRoot(classfile, m_classfile);
}

Class::Class(VM& vm, GC::Root<ClassFile>& classfile)
	: m_vm(vm)
{
	vm.gc().makeRoot(classfile, m_classfile);
}

void Class::name(GC::Root<char>& name)
{
	u16 thisClass = m_classfile.get().thisClass;
	ConstantPoolClass& pool = m_classfile.get().constantPool->get(thisClass).c_class;
	u16 nameIndex = pool.nameIndex;
	ConstantPoolUtf8& string = m_classfile.get().constantPool->get(nameIndex).c_utf8;
	m_vm.gc().makeRoot(string.bytes, name);
}
