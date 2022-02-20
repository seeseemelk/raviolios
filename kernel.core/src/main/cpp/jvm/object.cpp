#include "jvm_defs.hpp"

using namespace Java;

void* JavaObject::getFieldStart()
{
	return &class_ + 1;
}

void JavaObject::describer(GC::Meta* object, GC::MetaVisitor& visitor)
{
	JavaObject* obj = object->as<JavaObject>();
	visitor.visit(&obj->class_);
}
