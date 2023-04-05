#pragma once

#include <defines.h>

void memcpy(void *dest, void *src, size_t n);
void memzero(void * s, uint64 n);
void* memset(void* s, int c, int len); 
void* sbrk(int increment);
