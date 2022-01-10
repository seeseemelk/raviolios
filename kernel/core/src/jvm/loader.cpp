#include "jvm_loader.hpp"

using namespace Java;

Loader::Loader(const u8* data, size_t length)
	: m_data(data)
	, m_length(length)
{
}

void Loader::advance(int amount)
{
	m_index += amount;
}

size_t Loader::position()
{
	return m_index;
}

u8 Loader::getU8(size_t index)
{
	if (index >= m_length)
		return 0;
	return m_data[index];
}

u16 Loader::getU16(size_t index)
{
	u16 high = getU8(index);
	u16 low = getU8(index + 1);
	return (high << 8) | low;
}

u32 Loader::getU32(size_t index)
{
	u32 high = getU16(index);
	u32 low = getU16(index + 2);
	return (high << 16) | low;
}

u8 Loader::readU8()
{
	u8 value = getU8(position());
	advance(1);
	return value;
}

u16 Loader::readU16()
{
	u16 value = getU16(position());
	advance(2);
	return value;
}

u32 Loader::readU32()
{
	u32 value = getU32(position());
	advance(4);
	return value;
}

void Loader::readBuf(u8* buf, size_t length)
{
	for (size_t i = 0; i < length; i++)
		buf[i] = readU8();
}

void Loader::readBuf(char* buf, size_t length)
{
	for (size_t i = 0; i < length; i++)
		buf[i] = readU8();
}
