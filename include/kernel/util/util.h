#pragma once

#include <defines.h>

#include <kernel/print/print.h>

#if 1
#define assert(statement) ((statement) ? (void)0 : printk("assertion failed in {#ff0000.s}:{d} {#ffffff.s}\n", __FILE__, __LINE__, statement))
#else
#define assert(statement) (void)0
#endif

size_t strlen(const char* str);
char* itoa(int num, char* buffer, int base);
char* uitoa(uint32 num, char* buffer, int base);

uint64 uRound(float64 x);
