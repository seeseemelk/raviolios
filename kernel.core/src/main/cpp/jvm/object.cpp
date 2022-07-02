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

	GC::Array<FieldRef>* fields = obj->class_->object.fields;
	for (size_t i = 0; i < fields->count(); i++)
	{
		FieldInfo& field = fields->get(i).field->object;
		if (field.type.isReferenceType())
		{
			GC::Meta** property = obj->getFieldAt<GC::Meta*>(field.offset);
			visitor.visit(property);
		}
	}
}
