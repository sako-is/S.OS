#pragma once

#include <defines.h>

typedef struct {
	uint16 limit_low;
	uint16 base_low;
	uint8  base_middle;
	uint8  access;
	uint8  granularity;
	uint8  base_high;
} __attribute__((packed)) GDT;

typedef struct {
	uint32 reserved0;
	uint64 rsp[3];
	uint64 reserved1;
	uint64 ist[7];
	uint64 reserved2;
	uint16 reserved3;
	uint16 iomap_base;
} __attribute__((packed)) TSS;

typedef struct {
	uint32 base_highest;
	uint32 reserved0;
} __attribute__((packed)) GDThigh;

typedef struct {
	uint16 limit;	// The upper 16 bits of all selector limits.
	uint32 base;	// The address of the first GDT struct.
} __attribute__((packed)) GDTptr;

typedef struct  {
	GDT     entries[6];
	GDThigh tss_extra;
	GDTptr  pointer;
	TSS     tss;
} __attribute__((packed)) __attribute__((aligned(0x10))) FullGDT;

void createDescriptor(uint8 access, uint8 flags);

void setupGDT();
