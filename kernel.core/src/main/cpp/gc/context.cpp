#include "gc.hpp"

#include "log.hpp"
#include "util.hpp"

using namespace GC;

/**
 * Gets the next object.
 *
 * @param meta The current object.
 *
 * @return The next object.
 */
Meta* nextObject(Meta* meta)
{
	u8* object = reinterpret_cast<u8*>(meta);
	object += sizeof(Meta) + meta->size;
	return reinterpret_cast<Meta*>(object);
}

void Context::init(void* memoryStart, size_t memorySize)
{
//	Log::trace("Initialised GC");
	m_memStart = static_cast<u8*>(memoryStart);
	m_memCurrent = static_cast<u8*>(memoryStart);
	m_memCapacity = memorySize;
}

void Context::deinit()
{
//	Log::trace("Freed GC");
}

size_t Context::getFree()
{
	return m_memCapacity - getUsed();
}

size_t Context::getUsed()
{
	size_t used = m_memCurrent - m_memStart;
	return used + m_permUsed;
}

void* Context::permAlloc(size_t size)
{
	/*if (m_memStart != m_memCurrent)
	{
		return NULL;
	}
	void* object = m_memStart;
	m_memStart += size;
	m_memCurrent = m_memStart;
	m_permUsed += size;
	return object;*/
	if (getFree() <= size)
		collect();
	if (getFree() <= size)
	{
		Log::error("Out of memory");
		Arch::panic();
		return nullptr;
	}

	u8* memEnd = m_memStart + m_memCapacity;
	void* object = memEnd - size;
	m_permUsed += size;
	return object;
}

AllocResult Context::allocateRaw(Meta& meta, RawRoot& root)
{
	size_t required = meta.size + sizeof(Meta);
	Log::tracef("Allocating object with size %d", required);
//	collect();
//	collect();
//	collect();
	if (getFree() < required)
		collect();
	if (getFree() < required)
	{
		Log::critical("Out of memory");
		Arch::panic();
		return AllocResult::OUT_OF_MEMORY;
	}
	createObject(meta, root);
	root.object->writeValidators();
	root.object->validate();
	Log::tracef("Free memory: %d bytes", getFree());
	return AllocResult::SUCCESS;
}

void Context::makeRootRaw(Meta* meta, RawRoot& root)
{
	root.clear();
	if (meta != nullptr)
		root.set(meta, &m_root, m_root.next);
}

void Context::createObject(Meta& meta, RawRoot& root)
{
	size_t size = meta.size + sizeof(Meta);
	u8* object = m_memCurrent;
	m_memCurrent += size;
	memorySet(object, static_cast<u8>(0), size);

	Meta* objectMeta = reinterpret_cast<Meta*>(object);
	object += sizeof(Meta);

	*objectMeta = meta;
	objectMeta->reachable = 0;
	m_objects++;

	makeRootRaw(objectMeta, root);
}

Meta* Context::firstObject()
{
	return reinterpret_cast<Meta*>(m_memStart);
}

class ValidatorVisitor : public MetaVisitor
{
public:
	void visit(Meta** /*object*/) override
	{
//		VALIDATE(*object);
	}

	void visitWeak(Meta** /*object*/) override
	{
//		VALIDATE(*object);
	}
};

void Context::validateAllInHeap()
{
	Meta* object = firstObject();
	size_t count = m_objects;
	while (count > 0)
	{
		object->validate();
		object = nextObject(object);
		count--;
	}
}

void Context::validateAllRoots()
{
	RawRoot* root = m_root.next;
	ValidatorVisitor visitor;
	while (root != nullptr)
	{
		visitor.visit(&root->object);
		root = root->next;
	}
}

void Context::collect()
{
	Log::trace("Collecting garbage");
	Log::trace("Marking");
	validateAllInHeap();
	validateAllRoots();
	mark();
	validateAllInHeap();
	validateAllRoots();
	Log::trace("Sweeping - update");
	sweepUpdate();
	validateAllInHeap();
	Log::trace("Sweeping - move");
	sweepMove();
	validateAllInHeap();
	validateAllRoots();
	Log::trace("Finished collecting garbage");
}

class MarkVisitor : public MetaVisitor
{
public:
	void visit(Meta** object) override
	{
		if (*object != nullptr) (*object)->validate();
		if (*object != nullptr && !(*object)->reachable)
		{
			(*object)->reachable = true;
			(*object)->describer(*object, *this);
		}
	}

	void visitWeak(Meta** object) override
	{
		if (*object != nullptr) (*object)->validate();
	}
};

void Context::mark()
{
	RawRoot* root = m_root.next;
	MarkVisitor visitor;
	while (root != nullptr)
	{
		root->object->validate();
		visitor.visit(&root->object);
		root = root->next;
	}
}

void Context::sweepUpdate()
{
	Meta* object = firstObject();
	size_t count = m_objects;
	size_t offset = 0;
	while (count > 0)
	{
		object->validate();
		if (!object->reachable)
		{
			offset += sizeof(Meta) + object->size;
			updateAddress(object, nullptr);
		}
		else if (offset > 0)
		{
			Meta* destination = reinterpret_cast<Meta*>(reinterpret_cast<u8*>(object) - offset);
			updateAddress(object, destination);
		}
		object->validate();

		count--;
		object = nextObject(object);
	}
}

class UpdateVisitor : public MetaVisitor
{
public:
	Meta* from;
	Meta* to;

	void visit(Meta** object) override
	{
		if (*object == from)
			*object = to;
	}

	void visitWeak(Meta** object) override
	{
		visit(object);
	}
};

void Context::updateAddress(Meta* from, Meta* to)
{
	updateAddressHeap(from, to);
	updateAddressRoots(from, to);
}

void Context::updateAddressHeap(Meta* from, Meta* to)
{
	UpdateVisitor visitor;
	visitor.from = from;
	visitor.to = to;

	Meta* object = firstObject();
	size_t count = m_objects;
	while (count > 0)
	{
		object->validate();
		if (object->reachable)
			object->describer(object, visitor);
		object->validate();
		count--;
		object = nextObject(object);
	}
}

void Context::updateAddressRoots(Meta* from, Meta* to)
{
	RawRoot* root = m_root.next;
	while (root != nullptr)
	{
		if (root->object == from)
			root->object = to;
		root = root->next;
	}
}

void Context::sweepMove()
{
	Meta* object = firstObject();
	size_t count = m_objects;
	size_t removed = 0;
	size_t offset = 0;
	u8* memEnd = m_memCurrent;
	while (count > 0)
	{
		Meta* next = nextObject(object);

		object->validate();
		if (!object->reachable)
		{
			size_t size = sizeof(Meta) + object->size;
			offset += size;
			m_memCurrent -= size;
			removed++;
		}
		else
		{
			object->reachable = 0;
			if (offset > 0)
			{
				u8* source = reinterpret_cast<u8*>(object);
				u8* destination = static_cast<u8*>(source) - offset;
				memoryMove(destination, source, sizeof(Meta) + object->size);
				object = reinterpret_cast<Meta*>(destination);
			}
		}
		object->validate();

		count--;
		object = next;
	}
	m_objects -= removed;
	u8 value = 0xAA;
	memorySet(m_memCurrent, value, static_cast<size_t>(memEnd - m_memCurrent));
	Log::infof("Removed %d objects (freed %d bytes)", removed, static_cast<size_t>(memEnd - m_memCurrent));
}
