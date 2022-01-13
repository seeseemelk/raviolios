#include "jvm_defs.hpp"

#include <cstring>

using namespace Java;

void ClassFile::describer(GC::Meta* object, GC::MetaVisitor& visitor)
{
	ClassFile* classfile = static_cast<ClassFile*>(object->getRaw());
	visitor.visit(&classfile->constantPool);
	visitor.visit(&classfile->interfaces);
	visitor.visit(&classfile->fields);
	visitor.visit(&classfile->methods);
	visitor.visit(&classfile->attributes);
}

u16 ClassFile::findMethodByName(const char* name) const
{
	size_t length = strlen(name);
	for (u16 i = 0; i < methodCount; i++)
	{
		MethodInfo& methodInfo = methods->get(i);
		u16 nameIndex = methodInfo.nameIndex;
		ConstantPoolUtf8& methodName = constantPool->get(nameIndex).c_utf8;

		if (length == methodName.length && strcmp(name, methodName.bytes->asPtr()) == 0)
			return i;
	}
	return -1;
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

void CodeAttribute::describer(GC::Meta* object, GC::MetaVisitor& visitor)
{
	CodeAttribute* code = static_cast<CodeAttribute*>(object->getRaw());
	visitor.visit(&code->code);
	visitor.visit(&code->exceptionTable);
	visitor.visit(&code->attributes);
}

void AttributeInfo::describe(GC::MetaVisitor& visitor)
{
	switch (attributeType)
	{
	case AttributeType::code:
		visitor.visit(&code);
		break;
	}
}

void AttributeInfo::describer(GC::Meta* object, GC::MetaVisitor& visitor)
{
	size_t count = object->size / sizeof(AttributeInfo);
	AttributeInfo* attributes = static_cast<AttributeInfo*>(object->getRaw());
	for (size_t i = 0; i < count; i++)
	{
		attributes[i].describe(visitor);
	}
}

void FieldInfo::describer(GC::Meta* object, GC::MetaVisitor& visitor)
{
	FieldInfo* field = static_cast<FieldInfo*>(object->getRaw());
	visitor.visit(&field->attributes);
}

AttributeInfo* MethodInfo::getAttributeOfType(AttributeType type)
{
	for (size_t i = 0; i < attributesCount; i++)
	{
		if (attributes->get(i).attributeType == type)
			return &attributes->get(i);
	}
	return nullptr;
}

void MethodInfo::describer(GC::Meta* object, GC::MetaVisitor& visitor)
{
	MethodInfo* method = static_cast<MethodInfo*>(object->getRaw());
	visitor.visit(&method->attributes);
}
