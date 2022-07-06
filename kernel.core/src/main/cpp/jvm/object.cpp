#include "jvm_defs.hpp"

using namespace Java;

void* JavaObject::getFieldStart()
{
	return static_cast<void*>(reinterpret_cast<u8*>(this) + sizeof(JavaObject));
}

void JavaObject::describer(GC::Meta* object, GC::MetaVisitor& visitor)
{
	JavaObject* obj = object->as<JavaObject>();
	visitor.visit(&obj->class_);

	size_t referenceProperties = obj->referencePropertiesCount;
	for (size_t i = 0; i < referenceProperties; i++)
	{
		GC::Meta** property = obj->getReferenceFieldAt(i);
		visitor.visit(property);
	}

//	GC::Array<FieldRef>* fields = obj->class_->object.fields;
//	for (size_t i = 0; i < fields->count(); i++)
//	{
//		FieldInfo& field = fields->get(i).field->object;
//		if (field.type.isReferenceType())
//		{
//			GC::Meta** property = obj->getFieldAt<GC::Meta*>(field.offset);
//			visitor.visit(property);
//		}
//	}
}
