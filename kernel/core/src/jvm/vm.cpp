#include "jvm_context.hpp"

#include "gc.hpp"
#include "jvm_defs.hpp"
#include "jvm_loader.hpp"
#include "log.hpp"
#include "util.hpp"

using namespace Java;

void VM::init(NativeClassLoader& classLoader, NativeMethod* nativeMethods, size_t nativeMethodCount)
{
	m_classLoader = &classLoader;
	m_nativeMethodCount = nativeMethodCount;
	m_nativeMethods = nativeMethods;
}

GC::Context& VM::gc()
{
	return m_gc;
}

void VM::allocateArray(GC::Root<u8>& root, size_t length)
{
	GC::Allocator<u8> meta(length);
	m_gc.allocate(meta, root);
}

void VM::allocateArray(GC::Root<char>& root, size_t length)
{
	GC::Allocator<char> meta(length);
	m_gc.allocate(meta, root);
}

void VM::allocateString(GC::Root<char>& root, const char* str)
{
	size_t length = stringLength(str);
	allocateArray(root, length);
	for (size_t i = 0; i < length; i++)
		root[i] = str[i];
}

ClassError VM::getClass(GC::Root<ClassFile>& classfile, const GC::Root<char> name)
{
	return m_classLoader->loadClass(*this, classfile, name);
}

ClassError VM::defineClass(GC::Root<ClassFile>& classfile, const u8* data, size_t length)
{
	Loader loader(data, length);

	GC::Allocator<ClassFile> meta;
	meta.describer = ClassFile::describer;
	m_gc.allocate(meta, classfile);

	// Load class file header
	classfile.get().magic = loader.readU32();
	classfile.get().minorVersion = loader.readU16();
	classfile.get().majorVersion = loader.readU16();
	classfile.get().constantPoolCount = loader.readU16() - 1;

	// Load constant pool
	GC::Allocator<ConstantPoolInfo> constantPoolMeta(classfile.get().constantPoolCount, ConstantPoolInfo::describer);
	GC::Root<ConstantPoolInfo> constantPoolRoot;
	m_gc.allocate(constantPoolMeta, constantPoolRoot);
	constantPoolRoot.store(&classfile.get().constantPool);

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
			array.store(&info->c_utf8.bytes);
			break;
		case CONSTANT_class:
			info->c_class.nameIndex = loader.readU16() - 1;
			break;
		case CONSTANT_fieldRef:
			info->c_field.classIndex = loader.readU16() - 1;
			info->c_field.nameAndTypeIndex = loader.readU16() - 1;
			break;
		case CONSTANT_methodRef:
			info->c_method.classIndex = loader.readU16() - 1;
			info->c_method.nameAndTypeIndex = loader.readU16() - 1;
			break;
		case CONSTANT_nameAndType:
			info->c_nameAndType.nameIndex = loader.readU16() - 1;
			info->c_nameAndType.descriptorIndex = loader.readU16() - 1;
			break;
		default:
//			printf("Unknown tag: %d at index %d (of %d)\n", info->tag, (int)i, (int)classfile.get().constantPoolCount);
			Log::info("Unknown tag");
			return ClassError::BAD_CONSTANT;
		}
	}

	// Continue loading class file header
	classfile.get().accessFlags = loader.readU16();
	u16 thisClass = loader.readU16() - 1;
	u16 superClass = loader.readU16() - 1;
	classfile.get().thisClass = thisClass;
	classfile.get().superClass = superClass;

	GC::Root<char> className;
	u16 classNameIndex = classfile.get().constantPool->get(thisClass).c_class.nameIndex;
	m_gc.makeRoot(classfile.get().constantPool->get(classNameIndex).c_utf8.bytes, className);

	// Load interfaces
	classfile.get().interfacesCount = loader.readU16();
	GC::Allocator<u16> interfacesMeta(classfile.get().interfacesCount);
	GC::Root<u16> interfacesRoot;
	m_gc.allocate(interfacesMeta, interfacesRoot);
	interfacesRoot.store(&classfile.get().interfaces);
	for (size_t i = 0; i < classfile.get().interfacesCount; i++)
		interfacesRoot[i] = loader.readU16();

	// Load fields
	classfile.get().fieldsCount = loader.readU16();
	GC::Allocator<FieldInfo> fieldInfoMeta(classfile.get().fieldsCount, FieldInfo::describer);
	GC::Root<FieldInfo> fieldInfoRoot;
	m_gc.allocate(fieldInfoMeta, fieldInfoRoot);
	fieldInfoRoot.store(&classfile.get().fields);
	for (size_t i = 0; i < classfile.get().fieldsCount; i++)
	{
		fieldInfoRoot[i].accessFlags = loader.readU16();
		u16 nameIndex = loader.readU16() - 1;
		fieldInfoRoot[i].nameIndex = nameIndex;
		fieldInfoRoot[i].descriptorIndex = loader.readU16();
		fieldInfoRoot[i].attributesCount = loader.readU16();

		fieldInfoRoot[i].name = classfile.get().constantPool->get(nameIndex).c_utf8.bytes;

		GC::Root<AttributeInfo> attributeInfo;
		loadAttributes(classfile, attributeInfo, loader, fieldInfoRoot[i].attributesCount);
		attributeInfo.store(&fieldInfoRoot[i].attributes);
	}

	// Load methods
	classfile.get().methodCount = loader.readU16();
	GC::Allocator<MethodRef> methodRefsMeta(classfile.get().methodCount, MethodRef::describer);
	GC::Root<MethodRef> methodRefs;
	m_gc.allocate(methodRefsMeta, methodRefs);
	methodRefs.store(&classfile.get().methods);
	for (size_t i = 0; i < classfile.get().methodCount; i++)
	{
		GC::Allocator<MethodInfo> methodInfoMeta(MethodInfo::describer);
		GC::Root<MethodInfo> methodInfo;
		m_gc.allocate(methodInfoMeta, methodInfo);
		methodInfo.store(&methodRefs[i].method);

		methodInfo.get().accessFlags = loader.readU16();
		methodInfo.get().nameIndex = loader.readU16() - 1;
		methodInfo.get().descriptorIndex = loader.readU16() - 1;

		u16 attributesCount = loader.readU16();
		methodInfo.get().attributesCount = attributesCount;
		classfile.store(&methodInfo.get().classFile);

		GC::Root<AttributeInfo> attributeInfo;
		loadAttributes(classfile, attributeInfo, loader, attributesCount);
		attributeInfo.store(&methodInfo.get().attributes);
	}

	// Load attributes
	u16 attributeCount = loader.readU16();
	classfile.get().attributesCount = attributeCount;
	GC::Root<AttributeInfo> attributeRoot;
	loadAttributes(classfile, attributeRoot, loader, attributeCount);
	attributeRoot.store(&classfile.get().attributes);

	// Load super class
	if (superClass != static_cast<u16>(-1))
	{
		GC::Root<ClassFile> superClassObj;
		GC::Root<char> superClassName;
		u16 superClassNameIndex = classfile.get().constantPool->get(superClass).c_class.nameIndex;
		m_gc.makeRoot(classfile.get().constantPool->get(superClassNameIndex).c_utf8.bytes, superClassName);
		ClassError error = getClass(superClassObj, superClassName);
		if (error != ClassError::GOOD)
			return error;
		superClassObj.store(&classfile.get().superClassObj);
	}

	return ClassError::GOOD;
}

void VM::loadAttributes(GC::Root<ClassFile>& classfile, GC::Root<AttributeInfo>& root, Loader& loader, size_t count)
{
	GC::Allocator<AttributeInfo> allocator(count, AttributeInfo::describer);
	m_gc.allocate(allocator, root);

	for (size_t i = 0; i < count; i++)
	{
		u16 attributeNameIndex = loader.readU16() - 1;
		u16 attributeLength = loader.readU32();
		root.get().attributeNameIndex = attributeNameIndex;
		root.get().attributeLength = attributeLength;

		GC::Array<char>* name = classfile.get().constantPool->get(attributeNameIndex).c_utf8.bytes;
		if (equals(name, "Code"))
		{
			root.get().attributeType = AttributeType::code;
			GC::Root<CodeAttribute> codeAttribute;
			loadCodeAttribute(classfile, codeAttribute, loader);
			codeAttribute.store(&root.get().code);
		}
		else
		{
			Log::warning("Unknown attribute, skipping");
			loader.advance(attributeLength);
		}
	}
}

void VM::loadCodeAttribute(GC::Root<ClassFile>& classfile, GC::Root<CodeAttribute>& root, Loader& loader)
{
	GC::Allocator<CodeAttribute> allocator(CodeAttribute::describer);
	m_gc.allocate(allocator, root);
	root.get().maxStack = loader.readU16();
	root.get().maxLocals = loader.readU16();
	u16 codeLength = loader.readU32();
	root.get().codeLength = codeLength;;

	GC::Allocator<Opcode> codeAllocator(codeLength);
	GC::Root<Opcode> codeRoot;
	m_gc.allocate(codeAllocator, codeRoot);
	for (size_t i = 0; i < codeLength; i++)
	{
		codeRoot[i].opcode = static_cast<Instruction>(loader.readU8());
	}
	codeRoot.store(&root.get().code);

	u16 exceptionLength = loader.readU16();
	root.get().exceptionTableLength = exceptionLength;
	GC::Allocator<ExceptionTableEntry> exceptionAllocator(exceptionLength);
	GC::Root<ExceptionTableEntry> exceptionRoot;
	m_gc.allocate(exceptionAllocator, exceptionRoot);
	exceptionRoot.store(&root.get().exceptionTable);
	for (size_t i = 0; i < exceptionLength; i++)
	{
		exceptionRoot[i].startPC = loader.readU16();
		exceptionRoot[i].endPC = loader.readU16();
		exceptionRoot[i].handlerPC = loader.readU16();
		exceptionRoot[i].catchType = loader.readU16();
	}

	u16 attributesCount = loader.readU16();
	GC::Root<AttributeInfo> attributesRoot;
	loadAttributes(classfile, attributesRoot, loader, attributesCount);
	attributesRoot.store(&root.get().attributes);
}
































