#include "asserts.hpp"

#include "gc.hpp"
#include <cstdlib>

using namespace GC;

struct DUT
{
	u8* area;
	Context context;

	DUT(size_t size = MB(1))
	{
		area = static_cast<u8*>(malloc(size));
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

	assertEquals(emptyDescriber, rootB.object->describer, "Describer of B has not changed");
	assertEquals(sizeB, rootB.object->size, "Size of B has not changed");
}


TEST("GC Stress Test")
{
	DUT dut(KB(4));

	for (int i = 0; i < 100; i++)
	{
		for (int i = 10; i < 500; i++)
		{
			Meta meta;
			meta.size = 17;
			RawRoot root;
			dut.context.allocateRaw(meta, root);
		}
		assertTrue(dut.context.getUsed() > 0, "Some objects are allocated before GC");
		dut.context.collect();
		assertTrue(dut.context.getUsed() == 0, "Heap is free after GC");
		dut.context.collect();
		dut.context.collect();
	}

//	for (int i = 0; i < 10000; i++)
//	{
//		Meta meta;
//		meta.size = 17;
//		RawRoot root;
//		dut.context.allocateRaw(meta, root);
//	}
}
