#include "jvm_type.hpp"

#include "log.hpp"
#include "arch.hpp"

using namespace Java;

static char* parseDescriptor(char* str, TypeDescriptor* descriptor)
{
	switch (*str)
	{
	case 'B':
		if (descriptor)
			descriptor->type = Type::BYTE;
		return str + 1;
	case 'C':
		if (descriptor)
			descriptor->type = Type::CHAR;
		return str + 1;
	case 'D':
		if (descriptor)
			descriptor->type = Type::DOUBLE;
		return str + 1;
	case 'F':
		if (descriptor)
			descriptor->type = Type::FLOAT;
		return str + 1;
	case 'I':
		if (descriptor)
			descriptor->type = Type::INTEGER;
		return str + 1;
	case 'J':
		if (descriptor)
			descriptor->type = Type::LONG;
		return str + 1;
	case 'S':
		if (descriptor)
			descriptor->type = Type::SHORT;
		return str + 1;
	case 'Z':
		if (descriptor)
			descriptor->type = Type::BOOLEAN;
		return str + 1;
	case 'V':
		if (descriptor)
			descriptor->type = Type::VOID;
		return str + 1;
	case '[':
		if (descriptor)
			descriptor->array = true;
		return parseDescriptor(str + 1, descriptor);
	case 'L':
		if (descriptor)
			descriptor->type = Type::REFERENCE;
		while (*str != ';')
			str++;
		return str + 1;
	default:
		Log::info("Odd descriptor");
		Arch::panic();
		return nullptr;
	}
}

TypeDescriptor TypeDescriptor::parse(GC::Root<char>& type)
{
	TypeDescriptor result = {};

	char* str = type.asPtr();
	if (type.count() == 0)
	{
		Log::info("Empty descriptor");
		Arch::panic();
	}

	if (str[0] == '(')
	{
		result.method = true;
		str++;

		while (*str != ')')
		{
			str = parseDescriptor(str, nullptr);
			result.arguments++;
		}
		str++;
		parseDescriptor(str, &result);

		return result;
	}

	Log::info("Unsupported type descriptor");
	Arch::panic();

	return result;
}

//TypeDescriptor TypeDescriptor::parseReturnType(GC::Root<char>& /*type*/)
//{
//	Arch::panic();
//}

TypeDescriptor TypeDescriptor::parseParameter(GC::Root<char>& /*type*/)
{
	Arch::panic();
}