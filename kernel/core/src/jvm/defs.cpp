#include "jvm_defs.hpp"

#include "jvm_thread.hpp"
#include "util.hpp"

using namespace Java;

void ClassFile::describer(GC::Meta* object, GC::MetaVisitor& visitor)
{
	ClassFile* classfile = static_cast<ClassFile*>(object->getRaw());
	visitor.visit(&classfile->constantPool);
	visitor.visit(&classfile->superClassObj);
	visitor.visit(&classfile->interfaces);
	visitor.visit(&classfile->fields);
	visitor.visit(&classfile->methods);
	visitor.visit(&classfile->attributes);
	visitor.visit(&classfile->vtable);
}

u16 ClassFile::findMethodByNameAndType(GC::Root<char>& name, GC::Root<char>& type) const
{
	for (u16 i = 0; i < methodCount; i++)
	{
		MethodInfo& methodInfo = methods->get(i).method->object;
		u16 nameIndex = methodInfo.nameIndex;
		u16 typeIndex = methodInfo.descriptorIndex;
		ConstantPoolUtf8& methodName = constantPool->get(nameIndex).c_utf8;
		ConstantPoolUtf8& methodType = constantPool->get(typeIndex).c_utf8;

		if (equals(name, methodName.bytes) && equals(type, methodType.bytes))
			return i;
	}
	return U16_MAX;
}

GC::Object<MethodInfo>* ClassFile::findMethodByNameAndTypeRecurse(GC::Root<char>& name, GC::Root<char>& type) const
{
	u16 method = findMethodByNameAndType(name, type);
	if (method != U16_MAX)
		return methods->get(method).method;
	if (superClassObj == nullptr)
		return nullptr;
	return superClassObj->object.findMethodByNameAndTypeRecurse(name, type);
}

u16 ClassFile::findMethodByName(const char* name) const
{
	size_t length = stringLength(name);
	for (u16 i = 0; i < methodCount; i++)
	{
		MethodInfo& methodInfo = methods->get(i).method->object;
		u16 nameIndex = methodInfo.nameIndex;
		ConstantPoolUtf8& methodName = constantPool->get(nameIndex).c_utf8;

		if (length == methodName.length && equals(methodName.bytes, name))
			return i;
	}
	return U16_MAX;
}

u16 ClassFile::findFieldByName(GC::Root<char>& name) const
{
	for (u16 i = 0; i < fieldsCount; i++)
	{
		FieldInfo& fieldInfo = fields->get(i).field->object;
		u16 nameIndex = fieldInfo.nameIndex;
		ConstantPoolUtf8& methodName = constantPool->get(nameIndex).c_utf8;

		if (equals(name, methodName.bytes))
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

//Opcode CodeAttribute::getOpcode(size_t index)
//{
//	return code->get(index);
//}

Instruction& CodeAttribute::getInstruction(size_t index)
{
	return instructions->get(index);
}

void CodeAttribute::describer(GC::Meta* object, GC::MetaVisitor& visitor)
{
	CodeAttribute* code = static_cast<CodeAttribute*>(object->getRaw());
	visitor.visit(&code->instructions);
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

bool FieldInfo::isStatic()
{
	return (accessFlags & ACC_STATIC) != 0;
}

void FieldInfo::describer(GC::Meta* object, GC::MetaVisitor& visitor)
{
	FieldInfo* field = object->as<FieldInfo>();
	visitor.visit(&field->attributes);
	visitor.visit(&field->name);
}

void FieldRef::describer(GC::Meta* object, GC::MetaVisitor& visitor)
{
	FieldRef* fields = object->as<FieldRef>();
	size_t count = object->count<FieldRef>();
	for (size_t i = 0; i < count; i++)
		visitor.visit(&(fields[i].field));
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

bool MethodInfo::isStatic()
{
	return (accessFlags & ACC_STATIC) != 0;
}

bool MethodInfo::isNative()
{
	return (accessFlags & ACC_NATIVE) != 0;
}

bool MethodInfo::isVirtual()
{
	return !isStatic() && !isNative();
}

void MethodInfo::describer(GC::Meta* object, GC::MetaVisitor& visitor)
{
	MethodInfo* method = object->as<MethodInfo>();
	visitor.visit(&method->attributes);
	visitor.visit(&method->classFile);
}

void MethodRef::describer(GC::Meta* object, GC::MetaVisitor& visitor)
{
	MethodRef* methodRef = object->as<MethodRef>();
	size_t count = object->count<MethodRef>();
	for (size_t i = 0; i < count; i++)
		visitor.visit(&(methodRef[i].method));
}
