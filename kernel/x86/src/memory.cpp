#include "memory.hpp"

#include "arch.hpp"
#include "log.hpp"
#include "math.hpp"
#include "multiboot.hpp"
#include "paging.hpp"

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

u8* Memory::g_heap = reinterpret_cast<u8*>(KB(4));
size_t Memory::g_heapSize = 0;

/**
 * Appends memory to the space allocated to the heap.
 *
 * @param memory The memory region to use for the heap.
 * @param length The number of bytes that are in the heap.
 */
//static void appendToHeap(u8* memory, size_t length)
//{
//	g_heapSize += length;
//}

/**
 * Gets the total amount of memory that can be used by the OS.
 *
 * @returns The number of bytes available.
 */
static size_t totalMemoryAvailable()
{
	Multiboot::MapIterator iterator;
	size_t bytes = 0;
	while (iterator.nextAvailable())
	{
		u32 start = Math::ceilg(iterator.entry->addr, KB(4));
		u32 end = Math::floorg(start + iterator.entry->len, KB(4));
		u32 useable = end - start;
		iterator.entry->addr = start;
		iterator.entry->len = useable;
		bytes += useable;
	}
	return bytes;
}

static void* allocateMemory(size_t amount)
{
	Multiboot::MapIterator iterator;
	while (iterator.nextAvailable())
	{
		if (iterator.entry->len >= amount)
		{
			void* ptr = reinterpret_cast<void*>(iterator.entry->addr);
			iterator.entry->addr += amount;
			iterator.entry->len -= amount;
			return ptr;
		}
	}
	Log::criticalf("Out of memory when allocating %d bytes", amount);
	Arch::panic();
	return nullptr;
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

	size_t bytes = totalMemoryAvailable();
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

	Log::infof("Have %d MiB (%d KiB) of heap memory", g_heapSize / MB(1), g_heapSize / KB(1));
}
