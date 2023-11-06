#pragma once

#include <defines.h>
#ifdef X86_64

/* 
typedef struct {
	uint16 limit_low;
	uint16 base_low;
	uint8  base_middle;
	uint8  access;
	uint8  granularity;
	uint8  base_high;
} attr(packed) GDTentry; 

typedef struct {
	uint32 reserved0;
	uint64 rsp[3];
	uint64 reserved1;
	uint64 ist[8];
	uint64 reserved2;
	uint16 reserved3;
	uint16 iomap_base;
} attr(packed) TSS;
Eventually if I need a TSS (also refer to https://github.com/klange/toaruos/blob/master/kernel/arch/x86_64/gdt.c for reference)
*/

typedef struct {
	uint16  size;	// The upper 16 bits of all selector limits.
	uint64* base;	// The address of the first GDT struct.
} attr(packed) GDTptr;

typedef struct  {
	uint64 entries[5];
	// TSS tss;
	GDTptr pointer;
} attr(packed) attr(aligned(0x10)) FullGDT;

extern FullGDT gdt;

void setupGDT();
#endif
