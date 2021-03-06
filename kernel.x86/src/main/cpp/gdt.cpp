#include "gdt.hpp"

using namespace GDT;

enum DescriptorType
{
	CODE_OR_DATA = 1,
	SYSTEM = 0
};

enum Direction
{
	GROWS_UP = 0,
	GROWS_DOWN = 1
};

enum Granularity
{
	BYTE = 0,
	PAGE = 1
};

enum RWCode
{
	ALLOW_READ = 1,
	DISALLOW_READ = 0
};

enum RWData
{
	ALLOW_WRITE = 1,
	DISALLOW_WRITE = 0
};

enum OperationSize
{
	BIT16 = 0,
	BIT32 = 1
};

struct SegmentDescriptor
{
	u16 limit00_15;
	u16 base00_15;
	u8 base16_23;

	u8 accessed : 1;
	u8 rw : 1;
	u8 direction : 1;
	u8 executable : 1;

	u8 descriptor_type : 1;
	u8 dpl : 2;
	u8 present : 1;

	u16 limit16_19 : 4;
	u16 available : 1;
	u16 bit64 : 1; /* Should always be 0 */
	u16 operation_size : 1;
	u16 granularity : 1;
	u16 base24_31 : 8;

	void set_base_limit(u32 address, u32 bytes);
} __attribute__((packed));

struct GDTDescriptor
{
	u16 limit;
	u32 base;
} __attribute__((packed));

static SegmentDescriptor gdt[3];
GDTDescriptor g_gdtDescriptor;

void SegmentDescriptor::set_base_limit(u32 address, u32 bytes)
{
	base00_15 = address & 0xFFFF;
	base16_23 = (address >> 16) & 0xFF;
	base24_31 = (address >> 24) & 0xFF;

	u32 limit = (address + bytes) / KB(4);
	limit00_15 = limit & 0xFFFF;
	limit16_19 = (limit >> 16);
}

static void load_kernel_segment_registers()
{
	asm(
			"jmp $0x08,$.jump\n"
			".jump:"
			"mov $0x10, %ax\n"
			"mov %ax, %ds\n"
			"mov %ax, %es\n"
			"mov %ax, %gs\n"
			"mov %ax, %fs\n"
			"mov %ax, %ss\n"
	);
}

void GDT::init()
{
	// Set null descriptor to 0
	gdt[0] = {};

	// Kernel code descriptor
	// 8 -> 0x08
	gdt[1].rw = RWCode::ALLOW_READ;
	gdt[1].direction = Direction::GROWS_UP;
	gdt[1].executable = true;
	gdt[1].descriptor_type = DescriptorType::CODE_OR_DATA;
	gdt[1].dpl = 0;
	gdt[1].present = 1;
	gdt[1].operation_size = OperationSize::BIT32;
	gdt[1].granularity = Granularity::PAGE;
	gdt[1].set_base_limit(0, GB(4) - 1);

	// Kernel data descriptor
	// 16 -> 0x10
	gdt[2].rw = RWData::ALLOW_WRITE;
	gdt[2].direction = Direction::GROWS_UP;
	gdt[2].executable = false;
	gdt[2].descriptor_type = DescriptorType::CODE_OR_DATA;
	gdt[2].dpl = 0;
	gdt[2].present = 1;
	gdt[2].operation_size = OperationSize::BIT32;
	gdt[2].granularity = Granularity::PAGE;
	gdt[2].set_base_limit(0, GB(4) - 1);

	// Setup GDT Descriptor
	g_gdtDescriptor.base = reinterpret_cast<u32>(&gdt);
	g_gdtDescriptor.limit = sizeof(gdt) - 1;
	asm("lgdt g_gdtDescriptor");
	load_kernel_segment_registers();
}
