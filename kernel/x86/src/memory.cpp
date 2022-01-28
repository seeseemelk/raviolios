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
		bytes += iterator.entry->len;
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
	return nullptr;
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

	// Map the meta table.
	Paging::PageEntry& entry = g_page_directory.last(-1);
	entry.rw = 1;
	entry.setAddressFromVirtual(&s_metaTable);
	entry.present = 1;

	// Allocate page tables
	size_t i = 1;
	while (tablesNeeded > 0 && i < 1023)
	{
		void* memory = allocateMemory(sizeof(Paging::PageTable));
		Paging::PageEntry& entry = g_page_directory.first(i);
		entry.rw = 1;
		entry.setAddress(memory);
		entry.present = 1;
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
		entryIndex++;
		g_heapSize += KB(4);
	}
}
