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
	if (m_memStart != m_memCurrent)
	{
		return NULL;
	}
	void* object = m_memStart;
	m_memStart += size;
	m_memCurrent = m_memStart;
	m_permUsed += size;
	return object;
}

AllocResult Context::allocateRaw(Meta& meta, RawRoot& root)
{
//	Log::trace("Allocating object");
	size_t required = meta.size + sizeof(Meta);
	if (getFree() < required)
		collect();
	if (getFree() < required)
		return AllocResult::OUT_OF_MEMORY;
	createObject(meta, root);
	return AllocResult::SUCCESS;
}

void Context::makeRootRaw(Meta* meta, RawRoot& root)
{
	root.clear();
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

void Context::collect()
{
//	Log::trace("Collecting garbage");
	mark();
	sweepUpdate();
	sweepMove();
}

class MarkVisitor : public MetaVisitor
{
public:
	void visit(Meta** object) override
	{
		if (*object != nullptr)
		{
			(*object)->reachable = true;
			(*object)->describer(*object, *this);
		}
	}

	void visitWeak(Meta** /*object*/) override
	{
	}
};

void Context::mark()
{
	RawRoot* root = m_root.next;
	MarkVisitor visitor;
	while (root != nullptr)
	{
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
		if (object->reachable)
			object->describer(object, visitor);
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
	size_t offset = 0;
	while (count > 0)
	{
		if (!object->reachable)
		{
			offset += sizeof(Meta) + object->size;
		}
		else if (offset > 0)
		{
			u8* source = reinterpret_cast<u8*>(object);
			u8* destination = static_cast<u8*>(source) - offset;
			memoryMove(destination, source, sizeof(Meta) + object->size);
			object = reinterpret_cast<Meta*>(object);
			m_objects--;
		}

		count--;
		object->reachable = 0;
		object = nextObject(object);
	}
}
