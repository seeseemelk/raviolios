ENTRY(_start)

SECTIONS
{
	_kernel_base = 0xFFC00000;
	. = 0xFFD00000;
	g_kernel_start = .;

	.text ALIGN(4K) : AT(ADDR(.text) - _kernel_base)
	{
		*(.multiboot)
		*(.text)
	}

	.rodata ALIGN(4K) : AT(ADDR(.rodata) - _kernel_base)
	{
		*(.rodata)
	}

	.data ALIGN(4K) : AT(ADDR(.data) - _kernel_base)
	{
		*(.data)
	}

	.bss ALIGN(4K) : AT(ADDR(.bss) - _kernel_base)
	{
		*(COMMON)
		*(.bss)
	}

	g_kernel_end = ALIGN(4K);
}
