#include "jvm_context.hpp"

#include "jvm_loader.hpp"
#include "jvm_defs.hpp"

#include <cstdio>
#include <cstring>

using namespace Java;

GC::Context& VM::gc()
{
	return m_gc;
}

void VM::allocateArray(GC::Root<u8>& root, size_t length)
{
	GC::Ref<u8> meta;
	meta.size = length;
	m_gc.allocate(meta, root);
}

void VM::allocateArray(GC::Root<char>& root, size_t length)
{
	GC::Ref<char> meta;
	meta.size = length;
	m_gc.allocate(meta, root);
}

ClassError VM::defineClass(GC::Root<ClassFile>& classfile, const u8* data, size_t length)
{
	Loader loader(data, length);

	GC::Ref<ClassFile> meta;
	meta.describer = ClassFile::describer;
	m_gc.allocate(meta, classfile);

	// Load class file header
	classfile.get().magic = loader.readU32();
	classfile.get().minorVersion = loader.readU16();
	classfile.get().majorVersion = loader.readU16();
	classfile.get().constantPoolCount = loader.readU16() - 1;

	// Load constant pool
	GC::Ref<ConstantPoolInfo> constantPoolMeta;
	constantPoolMeta.size = classfile.get().constantPoolCount * sizeof(ConstantPoolInfo);
	constantPoolMeta.describer = ConstantPoolInfo::describer;
	GC::Root<ConstantPoolInfo> constantPoolRoot;
	m_gc.allocate(constantPoolMeta, constantPoolRoot);
	constantPoolRoot.storeRef(&classfile.get().constantPool);

	GC::Root<char> array;

	for (size_t i = 0; i < classfile.get().constantPoolCount; i++)
	{
		ConstantPoolInfo* info = &constantPoolRoot[i];
		info->tag = loader.readU8();
		switch (info->tag)
		{
		case CONSTANT_utf8:
			info->c_utf8.length = loader.readU16();
			allocateArray(array, info->c_utf8.length);
			info = &constantPoolRoot[i];
			loader.readBuf(&array.get(), info->c_utf8.length);
			array.storeRef(&info->c_utf8.bytes);
			break;
		case CONSTANT_class:
			info->c_class.nameIndex = loader.readU16();
			break;
		case CONSTANT_fieldRef:
			info->c_field.classIndex = loader.readU16();
			info->c_field.nameAndTypeIndex = loader.readU16();
			break;
		case CONSTANT_methodRef:
			info->c_method.classIndex = loader.readU16();
			info->c_method.nameAndTypeIndex = loader.readU16();
			break;
		case CONSTANT_nameAndType:
			info->c_nameAndType.nameIndex = loader.readU16();
			info->c_nameAndType.descriptorIndex = loader.readU16();
			break;
		default:
			printf("Unknown tag: %d at index %d (of %d)\n", info->tag, (int)i, (int)classfile.get().constantPoolCount);
			return ClassError::BAD_CONSTANT;
		}
	}

	// Continue loading class file header
	classfile.get().accessFlags = loader.readU16();
	classfile.get().thisClass = loader.readU16();
	classfile.get().superClass = loader.readU16();

	// Load interfaces
	classfile.get().interfacesCount = loader.readU16();
	GC::Ref<u16> interfacesMeta;
	interfacesMeta.size = classfile.get().interfacesCount * sizeof(u16);
	GC::Root<u16> interfacesRoot;
	m_gc.allocate(interfacesMeta, interfacesRoot);
	interfacesRoot.storeRef(&classfile.get().interfaces);
	for (size_t i = 0; i < classfile.get().interfacesCount; i++)
		interfacesRoot[i] = loader.readU16();

	// Load fields
	classfile.get().fieldsCount = loader.readU16();
	GC::Ref<FieldInfo> fieldInfoMeta;
	GC::Root<FieldInfo> fieldInfoRoot;
	fieldInfoMeta.describer = FieldInfo::describer;
	fieldInfoMeta.size = classfile.get().fieldsCount * sizeof(FieldInfo);
	m_gc.allocate(fieldInfoMeta, fieldInfoRoot);
	fieldInfoRoot.storeRef(&classfile.get().fields);
	for (size_t i = 0; i < classfile.get().fieldsCount; i++)
	{
		fieldInfoRoot[i].accessFlags = loader.readU16();
		u16 nameIndex = loader.readU16() - 1;
		fieldInfoRoot[i].nameIndex = nameIndex;
		fieldInfoRoot[i].descriptorIndex = loader.readU16();
		fieldInfoRoot[i].attributesCount = loader.readU16();

		fieldInfoRoot[i].name = classfile.get().constantPool[nameIndex].get().c_utf8.bytes;

		GC::Root<AttributeInfo> attributeInfo;
		loadAttributes(classfile, attributeInfo, loader, fieldInfoRoot[i].attributesCount);
		attributeInfo.storeRef(&fieldInfoRoot[i].attributes);
	}

	// Load methods
	// Load attributes

	return ClassError::GOOD;
}

void VM::loadAttributes(GC::Root<ClassFile>& classfile, GC::Root<AttributeInfo>& root, Loader& loader, size_t count)
{
	for (size_t i = 0; i < count; i++)
	{
		u16 attributeNameIndex = loader.readU16() - 1;
		root.get().attributeNameIndex = attributeNameIndex;
		root.get().attributeLength = loader.readU32();

		char* name = &classfile.get().constantPool[attributeNameIndex].get().c_utf8.bytes->get();
//		if (strcmp(name, "Code") == 0)
//		{
//			root.get().attributeType = AttributeType::code;
//		}
//		else
//		{
			printf("Unknown attribute: %s\n", name);
			return;
//		}
	}
}

// ViEwEr PaRtIcIpaTiOn
































