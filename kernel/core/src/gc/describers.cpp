#include "gc.hpp"

using namespace GC;

void GC::emptyDescriber(Meta* /*object*/, MetaVisitor& /*visitor*/)
{
}

void GC::arrayDescriber(Meta* object, MetaVisitor& visitor)
{
	Meta** array = reinterpret_cast<Meta**>(object->getRaw());
	size_t count = object->size / sizeof(Meta*);
	for (size_t i = 0; i < count; i++)
	{
		visitor.visit(array + i);
	}
}
