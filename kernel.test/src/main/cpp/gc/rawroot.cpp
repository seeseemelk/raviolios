#include "asserts.hpp"

#include "gc.hpp"

using namespace GC;

TEST("Can create root")
{
	Meta meta;
	RawRoot root;
	root.set(&meta, nullptr, nullptr);
	assertEquals(&meta, root.object, "Object equals meta");
	assertNull(root.next, "Next is null");
	assertNull(root.previous, "Previous is null");
}

TEST("Can chain roots")
{
	Meta metaA;
	Meta metaB;
	Meta metaC;

	RawRoot rootA;
	RawRoot rootB;
	RawRoot rootC;

	rootA.set(&metaA, nullptr, nullptr);
	rootB.set(&metaB, nullptr, &rootA);
	rootC.set(&metaC, nullptr, &rootB);

	assertEquals(&metaA, rootA.object, "Root A has the right object");
	assertEquals(&metaB, rootB.object, "Root B has the right object");
	assertEquals(&metaC, rootC.object, "Root C has the right object");

	assertNull(rootA.next, "Root A has no next");
	assertEquals(&rootB, rootA.previous, "Root A points back to B");
	assertEquals(&rootA, rootB.next, "Root B points forward to A");
	assertEquals(&rootC, rootB.previous, "Root B points back to C");
	assertEquals(&rootB, rootC.next, "Root C points forward to B");
	assertNull(rootC.previous, "Root C has no previous");
}

TEST("Can clear root twice")
{
	Meta metaA;
	Meta metaB;
	Meta metaC;

	RawRoot rootA;
	RawRoot rootB;
	RawRoot rootC;

	rootA.set(&metaA, nullptr, nullptr);
	rootB.set(&metaB, nullptr, &rootA);
	rootC.set(&metaC, nullptr, &rootB);

	rootB.clear();
	rootA.set(&metaA, nullptr, nullptr);
	rootB.clear();
	rootC.clear();
}
