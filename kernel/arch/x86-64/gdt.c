#include <kernel/arch/x86-64/gdt.h>
#include <kernel/util.h>

#include <defines.h>
#ifdef X86_64

// Access bits
#define PRESENT   1 << 7
#define NOT_SYS   1 << 4
#define EXEC      1 << 3
#define DC        1 << 2
#define RW        1 << 1
#define ACCESSED  1 << 0
// Flag bits
#define GRAN_4K   1 << 7
#define SZ_32     1 << 6
#define LONG_MODE 1 << 5

extern void loadGDT();

FullGDT gdt[32] __attribute__((used)) = {{
	{
		{ 0x0000, 0x0000, 0x00, 0x00, 0x00,                           0x00 },
		{ 0xFFFF, 0x0000, 0x00, 0x9A, (LONG_MODE) | (GRAN_4K) | 0x0F, 0x00 },
		{ 0xFFFF, 0x0000, 0x00, 0x92, (LONG_MODE) | (GRAN_4K) | 0x0F, 0x00 },
		{ 0xFFFF, 0x0000, 0x00, 0xFA, (LONG_MODE) | (GRAN_4K) | 0x0F, 0x00 },
		{ 0xFFFF, 0x0000, 0x00, 0xF2, (LONG_MODE) | (GRAN_4K) | 0x0F, 0x00 },
		{ 0x0067, 0x0000, 0x00, 0xE9, 0x00,                           0x00 },
	},
	{0x00000000, 0x00000000},
	{0x0000,     0x0000000000000000},
	{0,{0,0,0},0,{0,0,0,0,0,0,0},0,0,0},
}};

void setupGDT() {
	for (int i = 1; i < 32; ++i) {
		memcpy(&gdt[i], &gdt[0], sizeof(*gdt));
	}

	for (int i = 0; i < 32; ++i) {
		gdt[i].pointer.limit = sizeof(gdt[i].entries)+sizeof(gdt[i].tss_extra)-1;
		gdt[i].pointer.base  = (uintptr_t)&gdt[i].entries;

		uintptr_t addr = (uintptr_t)&gdt[i].tss;
		gdt[i].entries[5].limit_low = sizeof(gdt[i].tss);
		gdt[i].entries[5].base_low = (addr & 0xFFFF);
		gdt[i].entries[5].base_middle = (addr >> 16) & 0xFF;
		gdt[i].entries[5].base_high = (addr >> 24) & 0xFF;
		gdt[i].tss_extra.base_highest = (addr >> 32) & 0xFFFFFFFF;
	}

	extern void * stack_top;
	gdt[0].tss.rsp[0] = (uintptr_t)&stack_top;

	__asm__ __volatile__(
		"mov %0, %%rdi\n"
		"lgdt (%%rdi)\n"
		"mov $0x10, %%ax\n"
		"mov %%ax, %%ds\n"
		"mov %%ax, %%es\n"
		"mov %%ax, %%ss\n"
		"mov $0x2b, %%ax\n"
		"ltr %%ax\n"
		: : "r"(&gdt[0].pointer)
	);

    // loadGDT();
}
#endif
