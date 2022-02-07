#include "asserts.hpp"

#include "gc.hpp"
#include <cstdlib>

using namespace GC;

struct DUT
{
	void* area;
	Context context;

	DUT(size_t size = MB(1))
	{
		area = malloc(size);
		context.init(area, size);
	}

	~DUT()
	{
		context.deinit();
		free(area);
	}
};

TEST("Can create a GC context")
{
	DUT dut;
	size_t free = dut.context.getFree();
	size_t used = dut.context.getUsed();
	assertTrue(free == MB(1), "Less than 1 megabyte is available");
	assertTrue(used == 0, "More than 0 bytes are being used");
}

TEST("Can allocate objects")
{
	DUT dut;
	Meta meta;
	meta.size = 4;
	RawRoot root;
	dut.context.allocateRaw(meta, root);

	assertNull(root.next, "Next root is null");
	assertEquals(4U, root.object->size, "Size is correct");
	assertNotEquals(&meta, root.object, "Meta is different");
}

Meta* allocateObject(DUT& dut, Meta& meta)
{
	RawRoot root;
	dut.context.allocateRaw(meta, root);
	return root.object;
}

TEST("Can perform collection cycles")
{
	DUT dut;
	Meta meta;
	meta.size = 4;

	RawRoot rootA {};
	RawRoot rootB {};
	dut.context.allocateRaw(meta, rootA);
	dut.context.allocateRaw(meta, rootB);

	Meta* objectA = rootA.object;
	Meta* objectB = rootB.object;

	size_t sizeA = objectA->size;
	size_t sizeB = objectB->size;

	// Before any collection
	assertEquals(objectB, rootB.object, "Object B has not moved");

	// Cycle 1, nothing removed because roots exist
	dut.context.collect();
	assertEquals(objectB, rootB.object, "Object B has not moved");

	// Cycle 2, A removed as its root has been cleared
	// B moved over A.
	rootA.clear();
	dut.context.collect();
	assertNotEquals(objectB, rootB.object, "Object B has moved");
	assertEquals(objectA, rootB.object, "Object B moved over object A");

	assertEquals(emptyDescriber, objectA->describer, "Describer of A has not changed");
	assertEquals(sizeA, objectA->size, "Size of A has not changed");

	assertEquals(emptyDescriber, objectB->describer, "Describer of B has not changed");
	assertEquals(sizeB, objectB->size, "Size of B has not changed");
}
