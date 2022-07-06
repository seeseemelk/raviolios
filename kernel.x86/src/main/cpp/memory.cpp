#include "memory.hpp"

#include "cpu.hpp"
#include "log.hpp"
#include "math.hpp"
#include "multiboot.hpp"
#include "paging.hpp"
#include "x86.hpp"

using namespace Memory;

struct MetaTable
{
	Paging::PageTable tables[1024];

	Paging::PageEntry& entry(size_t i)
	{
		size_t table = i / 1024;
		size_t entry = i % 1024;
		return tables[table].entries[entry];
	}
};
static Paging::PageTable s_metaTable;
static MetaTable& s_tables = *reinterpret_cast<MetaTable*>(0xFF800000);

static u8* s_memoryBitMap;
static size_t s_pageCount;

u8* Memory::g_heap = reinterpret_cast<u8*>(MB(4));
size_t Memory::g_heapSize = 0;

/**
 * Gets the highest available address that is referenced by the multiboot header.
 */
static u32 highestAvailableAddress()
{
	u32 address = 0;
	Multiboot::MapIterator iterator;
	while (iterator.nextAvailable())
	{
		if (iterator.entry->type == MULTIBOOT_MEMORY_AVAILABLE)
		{
			u32 endAddress = iterator.entry->addr + iterator.entry->len;
			if (endAddress > address)
				address = endAddress;
		}
	}
	return address;
}

static bool isPageFree(size_t page)
{
	u32 offset = page / 8;
	u32 bit = page % 8;
	u8 mask = 1 << bit;
	return (s_memoryBitMap[offset] & mask) == 0;
}

static void markPageFree(size_t page)
{
	u32 offset = page / 8;
	u32 bit = page % 8;
	s_memoryBitMap[offset] &= ~(1 << bit);
}

static void markPageUsed(size_t page)
{
	u32 offset = page / 8;
	u32 bit = page % 8;
	s_memoryBitMap[offset] |= 1 << bit;
}

static void markUsedAA(u32 start, u32 end)
{
	size_t kernelStart = start / KB(4);
	size_t kernelEnd = Math::ceildiv(end, KB(4));
	for (size_t page = kernelStart; page < kernelEnd; page++)
		markPageUsed(page);
}

static void markUsedAL(u32 start, size_t length)
{
	markUsedAA(start, start + length);
}

template<typename T>
static void markUsedAL(T* start, size_t length)
{
	markUsedAL(reinterpret_cast<u32>(start), length);
}

static u32 getFreeMemory()
{
	size_t bytesFree = 0;
	for (size_t page = 0; page < s_pageCount; page++)
	{
		if (isPageFree(page))
			bytesFree += KB(4);
	}
	return bytesFree;
}

static void* allocateMemoryDirect(size_t amount)
{
	Multiboot::MapIterator iterator;
	while (iterator.nextAvailable())
	{
		multiboot_memory_map_t& entry = *iterator.entry;
		u32 addr = static_cast<u32>(entry.addr);
		size_t len = static_cast<size_t>(entry.len);
		if (!Arch::isInPhysicalKernel(addr, len) && iterator.entry->len >= amount)
		{
			void* ptr = reinterpret_cast<void*>(addr);
			entry.addr += amount;
			entry.len -= amount;
			return ptr;
		}
	}
	Log::criticalf("Out of memory when allocating %d bytes directly", amount);
	Arch::panic();
	return nullptr;
}

static void initBitMap()
{
	u32 highestAddress = highestAvailableAddress();
	s_pageCount = Math::ceildiv(highestAddress, KB(4));
	s_memoryBitMap = static_cast<u8*>(allocateMemoryDirect(s_pageCount / 8));

	for (size_t i = 0; i < s_pageCount; i++)
		markPageUsed(i);

	// Mark free pages
	Multiboot::MapIterator iterator;
	while (iterator.nextAvailable())
	{
		u32 start = Math::ceilg(iterator.entry->addr, KB(4)) / KB(4);
		u32 end = Math::floorg(iterator.entry->addr + iterator.entry->len, KB(4)) / KB(4);
		for (size_t page = start; page < end; page++)
			markPageFree(page);
	}

	// Mark kernel pages as used
	markUsedAA(Arch::kernelStart(), Arch::kernelEnd());
	markUsedAL(Multiboot::mbt, sizeof(multiboot_info_t));

	if (Multiboot::hasMultibootFlag(MULTIBOOT_INFO_MODS) && Multiboot::mbt->mods_count > 0)
	{
		multiboot_module_t* mmod = reinterpret_cast<multiboot_module_t*>(Multiboot::mbt->mods_addr);
		markUsedAL(mmod, sizeof(multiboot_module_t*));
	}
}

static void dumpMemoryMap()
{
	Log::info("Memory map:");
	size_t count = 0;
	Multiboot::MapIterator iterator;
	while (iterator.next())
	{
		u32 address = iterator.entry->addr;
		u32 length = iterator.entry->len;
		u32 end = address + length;
		count++;
		Log::infof(" - %x - %x (%d : %d KiB)", address, end, iterator.entry->type, length / KB(4));
	}
	Log::infof(" (%d items)", count);
}

static void* allocateMemory(size_t bytes)
{
	void* address = 0;
	size_t pages = 0;
	size_t neededPages = Math::ceildiv(bytes, KB(4));
	for (size_t i = 0; i < s_pageCount; i++)
	{
		if (isPageFree(i))
		{
			if (pages == 0)
				address = reinterpret_cast<void*>(i * KB(4));
			pages++;
			if (pages >= neededPages)
			{
				markUsedAL(address, bytes);
				return address;
			}
		}
		else
		{
			pages = 0;
		}
	}
	Log::criticalf("Out of memory when allocating %d bytes", bytes);
	Arch::panic();
}

Paging::PageTable& Paging::getPageTableForAddress(GC::Context& gc, u32 address)
{
	Paging::PageEntry& metaEntry = s_metaTable.first(address / MB(4));
	if (!metaEntry.present)
	{
		void* newTable = gc.permAlloc(sizeof(Paging::PageTable));
		Paging::PageEntry& originalEntry = getPageEntryForAddress(gc, reinterpret_cast<u32>(newTable));
		u32 tableAddress = originalEntry.getAddress();

		// Configure the page directory to use the table.
		Paging::PageEntry& entry = g_page_directory.first(address / MB(4));
		entry.setAddress(tableAddress);
		entry.rw = 1;
		entry.present = 1;

		// Map it in the meta table.
		metaEntry.setAddress(tableAddress);
		metaEntry.rw = 1;
		metaEntry.present = 1;

		// Clear the table.
		Paging::PageTable* asPageTable = static_cast<Paging::PageTable*>(newTable);
		*asPageTable = {};
	}
	return s_tables.tables[address / MB(4)];
}

Paging::PageEntry& Paging::getPageEntryForAddress(GC::Context& gc, u32 address)
{
	return getPageTableForAddress(gc, address).entries[(address % MB(4)) / KB(4)];
}

void Memory::init()
{
	Log::info("Initialising memory");
	if (!Multiboot::hasMultibootFlag(MULTIBOOT_INFO_MEM_MAP))
	{
		Log::critical("No memory map provided");
		Arch::panic();
	}
	dumpMemoryMap();

//	initIndirectMap();
	initBitMap();

	size_t bytes = getFreeMemory();
	size_t pagesNeeded = Math::ceildiv(bytes, KB(4));
	size_t tablesNeeded = Math::ceildiv(pagesNeeded, 1024);
	pagesNeeded -= tablesNeeded;

	for (size_t i = 0; i < 1024; i++)
		s_metaTable.entries[i] = {};

	// Map the meta table.
	u32 dirAddr = reinterpret_cast<u32>(&g_page_directory);
	Paging::PageTable* table = reinterpret_cast<Paging::PageTable*>(dirAddr - 0xFFC00000);
	Paging::PageEntry& entry = table->last(1);
	entry.setAddressFromVirtual(&s_metaTable);
	entry.rw = 1;
	entry.present = 1;

	// Allocate page tables
	size_t i = 1;
	while (tablesNeeded > 0 && i < 1023)
	{
		void* newTable = allocateMemory(sizeof(Paging::PageTable));

		// Clear the table.
		Paging::PageTable* asPageTable = static_cast<Paging::PageTable*>(newTable);
		*asPageTable = {};

		// Configure the page directory to use the table.
		Paging::PageEntry& entry = g_page_directory.first(i);
		entry.setAddress(newTable);
		entry.rw = 1;
		entry.present = 1;

		// Map it in the meta table.
		Paging::PageEntry& metaEntry = s_metaTable.first(i);
		metaEntry.setAddress(newTable);
		metaEntry.rw = 1;
		metaEntry.present = 1;

		i++;
		tablesNeeded--;
	}

	// Map pages
	size_t entryIndex = 1024;
	while (pagesNeeded > 0)
	{
		void* memory = allocateMemory(KB(4));
		Paging::PageEntry& entry = s_tables.entry(entryIndex);
		entry.setAddress(memory);
		entry.rw = 1;
		entry.present = 1;
		g_heapSize += KB(4);
		entryIndex++;
		pagesNeeded--;
	}
	g_heapSize = KB(32);

	// Unmap low pages
	g_page_directory.entries[0].present = 0;
	reloadPageTable();

	for (size_t i = 0; i < g_heapSize; i++)
		g_heap[i] = 0x55;

	Log::infof("Have %d MiB (%d KiB) of heap memory", g_heapSize / MB(1), g_heapSize / KB(1));
//	Arch::panic();
}
