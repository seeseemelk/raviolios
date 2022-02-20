#include "jvm_defs.hpp"

using namespace Java;

u8* JavaArray::firstValue()
{
	u8* obj = reinterpret_cast<u8*>(this);
	return obj + sizeof(JavaArray);
}

template<>
char& JavaArray::elementAt<char>(size_t index)
{
	return *reinterpret_cast<char*>(firstValue() + index);
}

template<>
GC::Meta*& JavaArray::elementAt<GC::Meta*>(size_t index)
{
	return *reinterpret_cast<GC::Meta**>(firstValue() + index * sizeof(GC::Meta**));
}

void JavaArray::describer(GC::Meta* object, GC::MetaVisitor& visitor)
{
	JavaArray& array = *object->as<JavaArray>();
	if (array.type != ArrayType::REFERENCE)
		return;
	for (size_t i = 0; i < array.length; i++)
	{
		visitor.visit(&array.elementAt<GC::Meta*>(i));
	}
}
