#include "icb.hpp"

#include "util.hpp"

using namespace ICB;

Bundle Bundle::from(u8* data, size_t size)
{
	Bundle bundle;
	bundle.m_data = data;
	bundle.m_size = size;
	return bundle;
}

bool Bundle::isICB1()
{
	return m_data[0] == 'I'
	    && m_data[1] == 'C'
	    && m_data[2] == 'B'
	    && m_data[3] == '1';
}

bool Bundle::next(Entry& entry)
{
	if (entry.tag == Tag::START)
	{
		readModule(entry, m_data + 4);
		return true;
	}

	u8* data = entry.content + entry.length;
	if (data >= (m_data + m_size))
		return false;

	readModule(entry, data);
	return true;
}

void Bundle::readModule(Entry& entry, u8* data)
{
	entry.tag = static_cast<Tag>(data[0]);
	entry.name = reinterpret_cast<const char*>(data + 1);
	size_t nameLength = stringLength(entry.name);
	entry.length = readU32(data + 1 + nameLength + 1);
	entry.content = data + 1 + nameLength + 1 + 4;
}

u32 Bundle::readU32(u8* data)
{
	u32 low = readU16(data);
	u32 high = readU16(data + 2);
	return (high << 16) | low;
}

u16 Bundle::readU16(u8* data)
{
	u16 low = data[0];
	u16 high = data[1];
	return (high << 8) | low;
}
