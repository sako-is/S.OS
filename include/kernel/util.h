#pragma once

#include <defines.h>

size_t strlen(const char* str);
char* itoa(int num, char* buffer, int base);
void memcpy(void *dest, void *src, size_t n);

uint64 uRound(float64 x);