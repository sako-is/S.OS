#pragma once

#include <defines.h>

typedef struct MemZone {
	union {
		uintptr start;
		uint8* ptr;
	};
	size_t len;
} MemZone;

void initMemZone();
void initMemZoneStage2();

MemZone newMemZone(size_t size);
MemZone newAlignedMemZone(size_t size, size_t alignment);
int freeMemZone(MemZone zone);
