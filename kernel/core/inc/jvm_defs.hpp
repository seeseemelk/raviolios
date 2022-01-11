/**@file
 * @brief
 */
#ifndef _JVM_DEFS_HPP_
#define _JVM_DEFS_HPP_

#include "defs.hpp"
#include "gc.hpp"

namespace Java
{
	struct ConstantPoolUtf8
	{
		u16 length;
		GC::Array<char>* bytes;
	};

	struct ConstantPoolClass
	{
		u16 nameIndex;
	};

	struct ConstantPoolFieldRef
	{
		u16 classIndex;
		u16 nameAndTypeIndex;
	};

	struct ConstantPoolMethodRef
	{
		u16 classIndex;
		u16 nameAndTypeIndex;
	};

	struct ConstantPoolNameAndType
	{
		u16 nameIndex;
		u16 descriptorIndex;
	};

	struct ConstantPoolInfo
	{
		u8 tag;
		union
		{
			ConstantPoolUtf8 c_utf8; /* Tag: 1 */
			// ConstantPoolInteger c_integer; /* Tag: 3 */
			// ConstantPoolFloat c_float; /* Tag: 4 */
			// ConstantPoolLong c_long; /* Tag: 5 */
			// ConstantPoolDouble c_double; /* Tag: 6 */
			ConstantPoolClass c_class; /* Tag: 7 */
			// ConstantPoolString c_string; /* Tag: 8 */
			ConstantPoolFieldRef c_field; /* Tag: 9 */
			ConstantPoolMethodRef c_method; /* Tag: 10 */
			// ConstantPoolInterfaceMethodRef c_interfaceMethod; /* Tag: 11 */
			ConstantPoolNameAndType c_nameAndType; /* Tag: 12 */
			// ConstantPoolMethodHandle c_methodHandle; /* Tag: 15 */
			// ConstantPoolMethodType c_methodType; /* Tag: 16 */
			// ConstantPoolDynamic c_dynamic; /* Tag: 17 */
			// ConstantPoolInvokeDynamic c_invokeDynamic; /* Tag: 18 */
			// ConstantPoolModule c_module; /* Tag: 19 */
			// ConstantPoolPackage c_package; /* Tag: 20 */
		};

		void describe(GC::MetaVisitor& visitor);
		static void describer(GC::Meta* object, GC::MetaVisitor& visitor);
	};

	enum ConstantPoolTag
	{
		CONSTANT_utf8 = 1,
		CONSTANT_integer = 3,
		CONSTANT_float = 4,
		CONSTANT_long = 5,
		CONSTANT_double = 6,
		CONSTANT_class = 7,
		CONSTANT_string = 8,
		CONSTANT_fieldRef = 9,
		CONSTANT_methodRef = 10,
		CONSTANT_interfaceMethodRef = 11,
		CONSTANT_nameAndType = 12,
		CONSTANT_methodHandle = 15,
		CONSTANT_methodType = 16,
		CONSTANT_dynamic = 17,
		CONSTANT_invokeDynamic = 18,
		CONSTANT_module = 19,
		CONSTANT_package = 20
	};

	enum class AttributeType
	{
//		constant_value,
		code,
//		stack_map_table,
		// Many others
	};

	struct ExceptionTableEntry
	{
		u16 startPC;
		u16 endPC;
		u16 handlerPC;
		u16 catchType;
	};

	struct Opcode
	{
		u8 opcode;
	};

	struct AttributeInfo;

	struct CodeAttribute
	{
		u16 maxStack;
		u16 maxLocals;
		u32 codeLength;
		GC::Array<Opcode>* code;
		u16 exceptionTableLength;
		GC::Array<ExceptionTableEntry>* exceptionTable;
		u16 attributesCount;
		GC::Array<AttributeInfo>* attributes;

		static void describer(GC::Meta* object, GC::MetaVisitor& visitor);
	};

	struct AttributeInfo
	{
		u16 attributeNameIndex;
		u32 attributeLength;
		AttributeType attributeType;

		union
		{
			GC::Object<CodeAttribute>* code;
		};

		void describe(GC::MetaVisitor& visitor);
		static void describer(GC::Meta* object, GC::MetaVisitor& visitor);
	};

	struct FieldInfo
	{
		u16 accessFlags;
		u16 nameIndex;
		u16 descriptorIndex;
		u16 attributesCount;
		GC::Array<AttributeInfo>* attributes;
		GC::Array<char>* name;

		static void describer(GC::Meta* object, GC::MetaVisitor& visitor);
	};

	struct MethodInfo
	{
		u16 accessFlags;
		u16 nameIndex;
		u16 descriptorIndex;
		u16 attributesCount;
		GC::Array<AttributeInfo>* attributes;

		static void describer(GC::Meta* object, GC::MetaVisitor& visitor);
	};

	struct ClassFile
	{
		u32 magic;
		u16 minorVersion;
		u16 majorVersion;
		u16 constantPoolCount;
		GC::Array<ConstantPoolInfo>* constantPool;
		u16 accessFlags;
		u16 thisClass;
		u16 superClass;
		u16 interfacesCount;
		GC::Array<u16>* interfaces;
		u16 fieldsCount;
		GC::Array<FieldInfo>* fields;
		u16 methodCount;
		GC::Array<MethodInfo>* methods;
		u16 attributesCount;
		GC::Array<AttributeInfo>* attributes;

		static void describer(GC::Meta* object, GC::MetaVisitor& visitor);
	};

	enum Access
	{
		ACC_PUBLIC = 0x0001,
		ACC_PRIVATE = 0x0002,
		ACC_PROTECTED = 0x0004,
		ACC_STATIC = 0x0008,
		ACC_FINAL = 0x0010,
		ACC_SUPER = 0x0020,
		ACC_VOLATILE = 0x0040,
		ACC_TRANSIENT = 0x0080,
		ACC_INTERACE = 0x0200,
		ACC_ABSTRACT = 0x0400,
		ACC_SYNTHETIC = 0x1000,
		ACC_ANNOTATION = 0x2000,
		ACC_ENUM = 0x4000,
		ACC_MODULE = 0x8000
	};
}

#endif /* _JVM_DEFS_HPP_ */
