#pragma once

#include <defines.h>

size_t strlen(const char* str);
char* itoa(int num, char* buffer, int base);
char* uitoa(uint32 num, char* buffer, int base);

uint64 uRound(float64 x);