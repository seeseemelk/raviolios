/**@file
 * @brief
 */
#ifndef _JVM_DEFS_HPP_
#define _JVM_DEFS_HPP_

#include "defs.hpp"
#include "gc.hpp"
#include "jvm_opcodes.hpp"
#include "jvm_type.hpp"

namespace Java
{
	struct ClassFile;
	struct Instruction;

	struct ConstantPoolUtf8
	{
		u16 length;
		GC::Array<char>* bytes;
	};

	struct ConstantPoolInteger
	{
		i32 integer;
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
			ConstantPoolInteger c_integer; /* Tag: 3 */
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

	struct JavaObject
	{
		GC::Object<ClassFile>* class_;

		void* getFieldStart();

		template<typename T>
		T* getFieldAt(size_t index)
		{
			return reinterpret_cast<T*>(static_cast<u8*>(getFieldStart()) + index);
		}

		static void describer(GC::Meta* object, GC::MetaVisitor& visitor);
	};

	struct Operand
	{
		union
		{
			i32 integer;
			GC::Object<JavaObject>* object;
			GC::Object<ClassFile>* classFile;
		};
		bool isObject = false;

		static void describer(GC::Meta* object, GC::MetaVisitor& visitor);
	};

	struct AttributeInfo;

	struct CodeAttribute
	{
		u16 maxStack;
		u16 maxLocals;
		u32 codeLength;
		GC::Array<Instruction>* instructions;
		u16 exceptionTableLength;
		GC::Array<ExceptionTableEntry>* exceptionTable;
		u16 attributesCount;
		GC::Array<AttributeInfo>* attributes;

//		Opcode getOpcode(size_t index);
		Instruction& getInstruction(size_t index);

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
		TypeDescriptor type;

		union
		{
			/// The value of the field, if it is a static field.
			Operand value;

			/// The offset into an object, if it is not a static field.
			i32 offset;
		};

		bool isStatic();

		static void describer(GC::Meta* object, GC::MetaVisitor& visitor);
	};

	struct FieldRef
	{
		GC::Object<FieldInfo>* field;

		static void describer(GC::Meta* object, GC::MetaVisitor& visitor);
	};

	struct MethodInfo
	{
		GC::Array<AttributeInfo>* attributes;
		GC::Object<ClassFile>* classFile;
		TypeDescriptor type;
		u16 accessFlags;
		u16 nameIndex;
		u16 descriptorIndex;
		u16 attributesCount;

		/**
		 * Gets an attribute of a specific type.
		 *
		 * @param type The type of attribute to get.
		 *
		 * @return The attribute, or `nullptr` if there is no attribute of the
		 * given type.
		 */
		AttributeInfo* getAttributeOfType(AttributeType type);

		/**
		 * Gets whether the method is a static method or not.
		 *
		 * @return `true` if the method is static, `false` if it is an instance method.
		 */
		bool isStatic();

		/**
		 * Gets whether the method is a native method or not.
		 *
		 * @return `true` if the method is native, `false` if it is a normal method.
		 */
		bool isNative();

		static void describer(GC::Meta* object, GC::MetaVisitor& visitor);
	};

	struct MethodRef
	{
		GC::Object<MethodInfo>* method;

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
		GC::Object<ClassFile>* superClassObj;
		u16 interfacesCount;
		GC::Array<u16>* interfaces;
		u16 fieldsCount;
		GC::Array<FieldRef>* fields;
		u16 methodCount;
		GC::Array<MethodRef>* methods;
		u16 attributesCount;
		GC::Array<AttributeInfo>* attributes;
		void* classLoader;

		/// The number of bytes that need to be allocated for an instance of the class.
		size_t objectSize;

		/**
		 * Finds a method by its name and type.
		 *
		 * @param name The name of the method.
		 * @param type The type of the method.
		 *
		 * @return An index into the @ref methods array. Contains a value of
		 * `-1` if the method was not found.
		 */
		u16 findMethodByNameAndType(GC::Root<char>& name, GC::Root<char>& type) const;

		/**
		 * Finds a method by its name.
		 *
		 * @param name The name of the method.
		 *
		 * @return An index into the @ref methods array. Contains a value of
		 * `-1` if the method was not found.
		 */
		u16 findMethodByName(const char* name) const;

		/**
		 * Finds a field by its name.
		 *
		 * @param name The name of the field.
		 *
		 * @return An index into the @ref fields array. Contains a value of `-1`
		 * if the field could not be found.
		 */
		u16 findFieldByName(GC::Root<char>& name) const;

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
		ACC_SYNCHRONIZED = 0x0020,
		ACC_VOLATILE = 0x0040,
		ACC_BRIDGE = 0x0040,
		ACC_TRANSIENT = 0x0080,
		ACC_VARARGS = 0x0080,
		ACC_NATIVE = 0x0100,
		ACC_INTERACE = 0x0200,
		ACC_ABSTRACT = 0x0400,
		ACC_STRICT = 0x0800,
		ACC_SYNTHETIC = 0x1000,
		ACC_ANNOTATION = 0x2000,
		ACC_ENUM = 0x4000,
		ACC_MODULE = 0x8000
	};
}

#endif /* _JVM_DEFS_HPP_ */
