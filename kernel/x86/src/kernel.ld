ENTRY(_start)

SECTIONS
{
	_kernel_base = 1M;
	. = 1M;
	_kernel_start = .;

	/* .text ALIGN(4K) : AT(ADDR(.text) - _kernel_base) */
	.text ALIGN(4K) :
	{
		*(.multiboot)
		*(.text)
	}

	/* .rodata ALIGN(4K) : AT(ADDR(.rodata) - _kernel_base) */
	.rodata ALIGN(4K) :
	{
		*(.rodata)
	}

	/* .data ALIGN(4K) : AT(ADDR(.data) - _kernel_base) */
	.data ALIGN(4K) :
	{
		*(.data)
	}

	/* .bss ALIGN(4K) : AT(ADDR(.bss) - _kernel_base) */
	.bss ALIGN(4K) :
	{
		*(COMMON)
		*(.bss)
	}

	_kernel_end = ALIGN(4K);
}
