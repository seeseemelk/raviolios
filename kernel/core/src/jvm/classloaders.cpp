#include "jvm_classloader.hpp"

#include "util.hpp"
#include "log.hpp"

using namespace Java;

void CachingClassLoader::parent(const NativeClassLoader& parentVtable, void* parent)
{
	m_parentVtable = &parentVtable;
	m_parent = parent;
}

static ClassError loadClass(void* arg, VM& vm, GC::Root<Java::Thread>& thread, GC::Root<ClassFile>& root, const GC::Root<char>& name)
{
	CachingClassLoader& classLoader = *static_cast<CachingClassLoader*>(arg);
	ClassList& list = classLoader.classList(vm);
	for (size_t i = 0; i < list.size; i++)
	{
		Log::trace("A");
		GC::Object<ClassFile>* classFile = list.entries->get(i).classFile;
		Log::trace("B");
		GC::Array<char>* entryName = list.entries->get(i).name;
		Log::trace("C1");
		bool a = classFile != nullptr;
		Log::trace("C2");
		bool b = equals(name, entryName);
		Log::trace("C3");
		if (a && b)
		{
			Log::trace("D");
			classFile->validate();
			Log::trace("E");
			vm.gc().makeRoot(classFile, root);
			Log::trace("F");
			return ClassError::GOOD;
		}
		Log::trace("G");
	}
	ClassError error = classLoader.m_parentVtable->loadClass(classLoader.m_parent, vm, thread, root, name);
	if (error != ClassError::GOOD)
		return error;

	ClassListEntry entry;
	name.store(&entry.name);
	root.store(&entry.classFile);
	entry.classFile->validate();
	ClassList::add(vm.gc(), classLoader.m_classList, entry);
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

const Java::NativeClassLoader Java::CachingClassLoader::vtable = {
		.loadClass = &loadClass
};

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
	size_t count = object->count<ClassListEntry>();
	ClassListEntry* infos = object->as<ClassListEntry>();
	for (size_t i = 0; i < count; i++)
	{
		if (infos[i].classFile != nullptr) infos[i].classFile->validate();
		visitor.visit(&infos[i].name);
		if (infos[i].classFile != nullptr) infos[i].classFile->validate();
		visitor.visitWeak(&infos[i].classFile);
		//infos->classFile->validate();
	}
}




