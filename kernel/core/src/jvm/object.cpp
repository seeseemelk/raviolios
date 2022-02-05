#include "jvm_defs.hpp"

using namespace Java;

void JavaObject::describer(GC::Meta* object, GC::MetaVisitor& visitor)
{
	JavaObject* obj = object->as<JavaObject>();
	visitor.visit(&obj->class_);
}
