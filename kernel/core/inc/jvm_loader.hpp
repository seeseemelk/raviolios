/**@file
 * @brief
 */
#ifndef _JVM_LOADER_HPP_
#define _JVM_LOADER_HPP_

#include "defs.hpp"
#include <cstddef>

namespace Java
{
	class Loader
	{
	public:
		Loader(const u8* data, size_t length);

		void advance(int amount);
		size_t position();

		u8 getU8(size_t index);
		u16 getU16(size_t index);
		u32 getU32(size_t index);

		u8 readU8();
		u16 readU16();
		u32 readU32();

		void readBuf(u8* buf, size_t length);
		void readBuf(char* buf, size_t length);

	private:
		const u8* m_data;
		size_t m_length;
		size_t m_index = 0;
	};
}

#endif /* _JVM_LOADER_HPP_ */
