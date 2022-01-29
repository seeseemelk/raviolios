/**@file
 * @brief
 */
#ifndef _PAGING_HPP_
#define _PAGING_HPP_

#include "defs.hpp"

namespace Paging
{
	struct PageEntry
	{
		u32 present : 1;
		u32 rw : 1;
		u32 userspace : 1;
		u32 pwt : 1;
		u32 pcd : 1;
		u32 accessed : 1;
		u32 dirty : 1;
		u32 pat : 1;
		u32 global : 1;
		u32 ignored : 3;
		u32 address : 20;

		template<typename T>
		void setAddress(T* address)
		{
			u32 value = reinterpret_cast<u32>(address);
			this->address = value >> 12;
		}

		template<typename T>
		void setAddressFromVirtual(T* address)
		{
			u32 value = reinterpret_cast<u32>(address) - 0xFFC00000;
			this->address = value >> 12;
		}
	};

	struct PageTable
	{
		PageEntry entries[1024];

		PageEntry& first(size_t n)
		{
			return entries[n];
		}

		PageEntry& last(size_t n)
		{
			return entries[1023 - n];
		}
	} __attribute__ ((aligned(4096)));
}

extern Paging::PageTable g_page_directory;
extern Paging::PageTable g_page_table;

#endif /* _PAGING_HPP_ */
