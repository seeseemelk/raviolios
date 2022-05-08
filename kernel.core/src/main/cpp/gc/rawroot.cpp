#include "gc.hpp"

#include "log.hpp"
#include "arch.hpp"

using namespace GC;

RawRoot::~RawRoot()
{
	clear();
}

void RawRoot::storeMeta(Meta** meta) const
{
	*meta = object;
}

void RawRoot::set(Meta* object, RawRoot* previous, RawRoot* next)
{
	clear();
	this->object = object;
	this->previous = previous;
	this->next = next;
	if (previous != nullptr)
		previous->next = this;
	if (next != nullptr)
		next->previous = this;
}

void RawRoot::clear()
{
	if (next != nullptr)
	{
		if (next->previous != this)
		{
			Log::critical("Bad next->previous");
			Arch::panic();
		}
		next->previous = previous;
	}
	if (previous != nullptr)
	{
		if (previous->next != this)
		{
			Log::critical("Bad previous->next");
			Arch::panic();
		}
		previous->next = next;
	}
	object = nullptr;
	next = nullptr;
	previous = nullptr;
}

//Root::Root(Meta* object, Root* previous, Root* next)
//	: object(object)
//	, previous(previous)
//	, next(next)
//{
//	std::cout << "Created root" << std::endl;
//	if (previous != nullptr)
//		previous->next = this;
//	if (next != nullptr)
//		next->previous = this;
//}
//
//Root::Root(const Root& root)
//	: Root(root.object, root.next, root.previous)
//{
//}
//
//Root::~Root()
//{
//	std::cout << "Removed root" << std::endl;
//	if (next != nullptr)
//		next->previous = previous;
//	if (previous != nullptr)
//		previous->next = next;
//}

//void Root::operator=(Root& root)
//{
//	previous = &root;
//	next = root.next;
//	root.next = this;
//}
//
//void Root::operator=(Root root)
//{
//	next = root.next;
//	previous = root.previous;
//}
