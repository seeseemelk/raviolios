#include "gc.hpp"

using namespace GC;

RawRoot::~RawRoot()
{
	clear();
}

void RawRoot::set(Meta* object, RawRoot* previous, RawRoot* next)
{
	this->object = object;
	this->previous = previous;
	this->next = next;
	if (previous != nullptr)
		previous->next = this;
	if (next != nullptr)
		next->previous = this;
}

void RawRoot::set(const RawRoot& root)
{
	clear();
	set(root.object, root.next, root.previous);
}

void RawRoot::clear()
{
	if (next != nullptr)
		next->previous = previous;
	if (previous != nullptr)
		previous->next = next;
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
