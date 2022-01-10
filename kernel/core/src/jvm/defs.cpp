#include "jvm_defs.hpp"

using namespace Java;

void ClassFile::describer(GC::Meta* /*object*/, GC::MetaVisitor& /*visitor*/)
{

}

void ConstantPoolInfo::describe(GC::MetaVisitor& visitor)
{
	switch (tag)
	{
	case CONSTANT_utf8:
		visitor.visit(&c_utf8.bytes);
		break;
	default:
		break;
	}
}

void ConstantPoolInfo::describer(GC::Meta* object, GC::MetaVisitor& visitor)
{
	size_t count = object->size / sizeof(ConstantPoolInfo);
	ConstantPoolInfo* infos = static_cast<ConstantPoolInfo*>(object->getRaw());
	for (size_t i = 0; i < count; i++)
	{
		infos[i].describe(visitor);
	}
}

void FieldInfo::describer(GC::Meta* object, GC::MetaVisitor& visitor)
{
	FieldInfo* field = static_cast<FieldInfo*>(object->getRaw());
	visitor.visit(&field->attributes);
}
