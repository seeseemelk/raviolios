#include "icb.hpp"

#include "util.hpp"

using namespace ICB;

void Loader::bundle(Bundle& bundle)
{
	m_bundle = bundle;
}

Java::ClassError Loader::loadClass(Java::VM& vm, GC::Root<Java::ClassFile>& root, const GC::Root<char>& name)
{
	Entry entry;
	while (m_bundle.next(entry))
	{
		if (equals(name, entry.name))
			return vm.defineClass(root, entry.content, entry.length);
	}
	return Java::ClassError::NOT_FOUND;
}
