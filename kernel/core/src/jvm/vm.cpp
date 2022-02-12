#include "jvm_context.hpp"

#include "gc.hpp"
#include "jvm_defs.hpp"
#include "jvm_loader.hpp"
#include "log.hpp"
#include "util.hpp"

using namespace Java;

void VM::init(const NativeClassLoader& classLoaderVtable, void* classLoader, const NativeMethod* nativeMethods, size_t nativeMethodCount)
{
	m_classLoaderVtable = &classLoaderVtable;
	m_classLoader = classLoader;
	m_nativeMethodCount = nativeMethodCount;
	m_nativeMethods = nativeMethods;
}

GC::Context& VM::gc()
{
	return m_gc;
}

void VM::allocateArray(GC::Root<u8>& root, size_t length)
{
	GC::Allocator<u8> meta(length, GC::emptyDescriber);
	m_gc.allocate(meta, root);
}

void VM::allocateArray(GC::Root<char>& root, size_t length)
{
	GC::Allocator<char> meta(length, GC::emptyDescriber);
	m_gc.allocate(meta, root);
}

void VM::allocateString(GC::Root<char>& root, const char* str)
{
	size_t length = stringLength(str);
	allocateArray(root, length);
	for (size_t i = 0; i < length; i++)
		root[i] = str[i];
}

ClassError VM::getClass(GC::Root<ClassFile>& classfile, GC::Root<Thread>& thread, const GC::Root<char>& name)
{
	return m_classLoaderVtable->loadClass(m_classLoader, *this, thread, classfile, name);
}

ClassError VM::defineClass(GC::Root<ClassFile>& classfile, GC::Root<Thread>& thread, const u8* data, size_t length)
{
	Loader loader(data, length);

	GC::Allocator<ClassFile> meta(ClassFile::describer);
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
		case CONSTANT_integer:
			info->c_integer.integer = loader.readU32();
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
	GC::Allocator<u16> interfacesMeta(classfile.get().interfacesCount, GC::emptyDescriber);
	GC::Root<u16> interfacesRoot;
	m_gc.allocate(interfacesMeta, interfacesRoot);
	interfacesRoot.store(&classfile.get().interfaces);
	for (size_t i = 0; i < classfile.get().interfacesCount; i++)
		interfacesRoot[i] = loader.readU16();

	// Load fields
	classfile.get().fieldsCount = loader.readU16();
	GC::Allocator<FieldRef> fieldRefMeta(classfile.get().fieldsCount, FieldRef::describer);
	GC::Root<FieldRef> fieldRefRoot;
	m_gc.allocate(fieldRefMeta, fieldRefRoot);
	fieldRefRoot.store(&classfile.get().fields);
	for (size_t i = 0; i < classfile.get().fieldsCount; i++)
	{
		GC::Allocator<FieldInfo> fieldInfoMeta(FieldInfo::describer);
		GC::Root<FieldInfo> fieldInfoRoot;
		m_gc.allocate(fieldInfoMeta, fieldInfoRoot);
		fieldInfoRoot.store(&fieldRefRoot[i].field);

		fieldInfoRoot[i].accessFlags = loader.readU16();
		u16 nameIndex = loader.readU16() - 1;
		fieldInfoRoot[i].nameIndex = nameIndex;
		u16 descriptorIndex = loader.readU16() - 1;
		fieldInfoRoot[i].descriptorIndex = descriptorIndex;
		fieldInfoRoot[i].attributesCount = loader.readU16();

		fieldInfoRoot[i].name = classfile.get().constantPool->get(nameIndex).c_utf8.bytes;

		GC::Root<char> descriptor;
		m_gc.makeRoot(constantPoolRoot[descriptorIndex].c_utf8.bytes, descriptor);
		fieldInfoRoot[i].type = TypeDescriptor::parse(descriptor);

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

	// Queue clinit method for execution, if there is one.
	u16 clinitMethod = classfile.get().findMethodByName("<clinit>");
	if (clinitMethod != U16_MAX)
	{
		invokeMethod(thread, classfile, clinitMethod, true);
	}

	// Load super class
	if (superClass != static_cast<u16>(-1))
	{
		GC::Root<ClassFile> superClassObj;
		GC::Root<char> superClassName;
		u16 superClassNameIndex = classfile.get().constantPool->get(superClass).c_class.nameIndex;
		m_gc.makeRoot(classfile.get().constantPool->get(superClassNameIndex).c_utf8.bytes, superClassName);
		ClassError error = getClass(superClassObj, thread, superClassName);
		if (error != ClassError::GOOD)
			return error;
		superClassObj.store(&classfile.get().superClassObj);
		classfile.get().objectSize = superClassObj.get().objectSize;
	}
	else
	{
		classfile.get().objectSize = sizeof(void*);
	}

	// Calculate size of object and alignment of fields.
	for (size_t i = 0; i < classfile.get().fieldsCount; i++)
	{
		FieldInfo& field = fieldRefRoot[i].field->object;
		if (!field.isStatic())
		{
			field.offset = classfile.get().objectSize;
			classfile.get().objectSize += field.type.size();
		}
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

	GC::Allocator<Instruction> codeAllocator(codeLength, Instruction::describer);
	GC::Root<Instruction> codeRoot;
	m_gc.allocate(codeAllocator, codeRoot);
	parseOpcodes(codeRoot, loader, codeLength);
//	for (size_t i = 0; i < codeLength; i++)
//	{
//		codeRoot[i].opcode = static_cast<Instruction>(loader.readU8());
//	}
//	codeRoot.store(&root.get().code);
	codeRoot.store(&root.get().instructions);

	u16 exceptionLength = loader.readU16();
	root.get().exceptionTableLength = exceptionLength;
	GC::Allocator<ExceptionTableEntry> exceptionAllocator(exceptionLength, GC::emptyDescriber);
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

void VM::parseOpcodes(GC::Root<Instruction>& instructions, Loader& loader, size_t instructionCount)
{
	size_t instructionIndex = 0;
	u16 instructionMap[instructionCount];
	for (size_t i = 0; i < instructionCount; i++)
	{
		u8 opcode = loader.readU8();
		Instruction instruction;
		switch (opcode)
		{
		case 0x03: /* iconst_0 */
			instruction.opcode = Opcode::iconst;
			instruction.constantInteger = 0;
			break;
		case 0x04: /* iconst_1 */
			instruction.opcode = Opcode::iconst;
			instruction.constantInteger = 1;
			break;
		case 0x05: /* iconst_2 */
			instruction.opcode = Opcode::iconst;
			instruction.constantInteger = 2;
			break;
		case 0x06: /* iconst_3 */
			instruction.opcode = Opcode::iconst;
			instruction.constantInteger = 3;
			break;
		case 0x07: /* iconst_4 */
			instruction.opcode = Opcode::iconst;
			instruction.constantInteger = 4;
			break;
		case 0x08: /* iconst_5 */
			instruction.opcode = Opcode::iconst;
			instruction.constantInteger = 5;
			break;
		case 0x10: /* bipush */
			instruction.opcode = Opcode::iconst;
			instruction.constantInteger = static_cast<i8>(loader.readU8());
			i += 1;
			break;
		case 0x11: /* sipush */
			instruction.opcode = Opcode::iconst;
			instruction.constantInteger = static_cast<i16>(loader.readU16());
			i += 2;
			break;
		case 0x1A: /* iload_0 */
			instruction.opcode = Opcode::iload;
			instruction.index = 0;
			i += 2;
			break;
		case 0x1B: /* iload_1 */
			instruction.opcode = Opcode::iload;
			instruction.index = 1;
			i += 2;
			break;
		case 0x1C: /* iload_2 */
			instruction.opcode = Opcode::iload;
			instruction.index = 2;
			i += 2;
			break;
		case 0x1D: /* iload_3 */
			instruction.opcode = Opcode::iload;
			instruction.index = 3;
			i += 2;
			break;
		case 0x2A: /* aload_0 */
			instruction.opcode = Opcode::aload;
			instruction.index = 0;
			break;
		case 0x2B: /* aload_1 */
			instruction.opcode = Opcode::aload;
			instruction.index = 1;
			break;
		case 0x2C: /* aload_2 */
			instruction.opcode = Opcode::aload;
			instruction.index = 2;
			break;
		case 0x2D: /* aload_3 */
			instruction.opcode = Opcode::aload;
			instruction.index = 3;
			break;
		case 0xA0: /* if_icmpne */
			instruction.opcode = Opcode::if_icmpne;
			break;
		case 0xAC: /* ireturn */
			instruction.opcode = Opcode::return_value;
			break;
		case 0xB1: /* return */
			instruction.opcode = Opcode::return_;
			break;
		case 0xB4: /* getfield */
			instruction.opcode = Opcode::getfield_a;
			instruction.index = loader.readU16() - 1;
			i += 2;
			break;
		case 0xB7: /* invokespecial */
		case 0xB8: /* invokestatic */
			instruction.opcode = Opcode::invoke_a;
			instruction.index = loader.readU16() - 1;
			i += 2;
			break;
		default:
			Log::criticalf("Unknown opcode: %b", opcode);
			Arch::panic();
		}
		instructions.asPtr()[instructionIndex++] = instruction;
	}
}































