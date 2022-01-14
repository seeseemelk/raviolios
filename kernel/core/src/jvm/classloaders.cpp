#include "jvm_classloader.hpp"

#include <cstring>

using namespace Java;

CachingClassLoader::CachingClassLoader(NativeClassLoader& parent)
	: m_parent(parent)
{
}

ClassError CachingClassLoader::loadClass(VM& vm, GC::Root<ClassFile>& root, const GC::Root<char>& name)
{
	ClassList& list = classList(vm);
	for (size_t i = 0; i < list.size; i++)
	{
		GC::Object<ClassFile>* classFile = list.entries->get(i).classFile;
		const char* entryName = list.entries->get(i).name->asPtr();
		if (classFile != nullptr && strcmp(entryName, name.asPtr()) == 0)
		{
			vm.gc().makeRoot(classFile, root);
			return ClassError::GOOD;
		}
	}
	ClassError error = m_parent.loadClass(vm, root, name);
	if (error != ClassError::GOOD)
		return error;

	ClassListEntry entry;
	name.store(&entry.name);
	root.store(&entry.classFile);
	ClassList::add(vm.gc(), m_classList, entry);
	return ClassError::GOOD;
}

ClassList& CachingClassLoader::classList(VM& vm)
{
	if (!m_classList.isSet())
	{
		ClassList::create(vm.gc(), m_classList);
	}
	return m_classList.get();
}

void ClassList::create(GC::Context& gc, GC::Root<ClassList>& root)
{
	GC::Allocator<ClassList> allocator(ClassList::describer);
	gc.allocate(allocator, root);

	root.get().capacity = 8;
	root.get().size = 0;

	GC::Allocator<ClassListEntry> listAllocator(root.get().capacity, ClassListEntry::describer);
	GC::Root<ClassListEntry> listRoot;
	gc.allocate(listAllocator, listRoot);
	listRoot.store(&root.get().entries);
}

void ClassList::add(GC::Context& gc, GC::Root<ClassList>& list, ClassListEntry& object)
{
	if (list.get().size == list.get().capacity)
	{
		size_t oldCapacity = list.get().capacity;
		size_t newCapacity = oldCapacity * 2;
		list.get().capacity = newCapacity;

		GC::Allocator<ClassListEntry> listAllocator(newCapacity, ClassListEntry::describer);
		GC::Root<ClassListEntry> listRoot;
		gc.allocate(listAllocator, listRoot);

		for (size_t i = 0; i < oldCapacity; i++)
			listRoot[i] = list.get().entries->get(i);
		listRoot.store(&list.get().entries);
	}
	list.get().entries->get(list.get().size) = object;
	list.get().size++;
}

void ClassList::describer(GC::Meta* object, GC::MetaVisitor& visitor)
{
	ClassList* infos = static_cast<ClassList*>(object->getRaw());
	visitor.visit(&infos->entries);
}

void ClassListEntry::describer(GC::Meta* object, GC::MetaVisitor& visitor)
{
	size_t count = object->size / sizeof(ClassListEntry);
	ClassListEntry* infos = static_cast<ClassListEntry*>(object->getRaw());
	for (size_t i = 0; i < count; i++)
	{
		visitor.visit(&infos->name);
		visitor.visitWeak(&infos->classFile);
	}
}



