#pragma once

#include <defines.h>
#include <limine/limine.h>

#define PAGE_SIZE 4096

extern volatile struct limine_memmap_request memmap_request;

void initPMM(void);
void* PMMalloc(size_t pages);
void* PMMallocNoZero(size_t pages);
void PMMfree(void* addr, size_t pages);
