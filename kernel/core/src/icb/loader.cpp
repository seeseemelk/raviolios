#include "icb.hpp"

#include "util.hpp"

using namespace ICB;

void Loader::bundle(Bundle& bundle)
{
	m_bundle = bundle;
}

static Java::ClassError loadClass(void* arg, Java::VM& vm, GC::Root<Java::ClassFile>& root, const GC::Root<char>& name)
{
	Loader& loader = *static_cast<Loader*>(arg);
	Entry entry;
	while (loader.m_bundle.next(entry))
	{
		if (equals(name, entry.name))
			return vm.defineClass(root, entry.content, entry.length);
	}
	return Java::ClassError::NOT_FOUND;
}

const Java::NativeClassLoader ICB::Loader::vtable = {
		.loadClass = &loadClass
};
