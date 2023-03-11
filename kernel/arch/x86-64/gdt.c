#include <defines.h>
#ifdef X86_64
#include <kernel/arch/x86-64/gdt.h>
#include <kernel/util/util.h>

// extern void loadTSS();

FullGDT gdt = {
	{
		0x0000000000000000, // null
		0x00af9b000000ffff, // 64-bit code
		0x00af93000000ffff, // 64-bit data
		0x00affb000000ffff, // usermode 64-bit code
		0x00aff3000000ffff, // usermode 64-bit data
	},
	{ 0x0000, 0x0000000000000000 },	// GDT pointer
};

void setupGDT() {
	gdt.pointer.size = sizeof(gdt.entries) - 1;
	gdt.pointer.base = &gdt.entries[0];

	asmvol(
		"mov %0, %%rdi\n"
		"lgdt (%%rdi)\n"
		:: "r"(&gdt.pointer)
	);
}
#endif