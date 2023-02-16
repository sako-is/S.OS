#include <kernel/util.h>

#include <defines.h>

size_t strlen(const char* str) {
    size_t len = 0;
    while(str[len]) len++;

    return len;
}

uint64 uRound(float64 x) {
    return (uint64)(x + 0.5);
}