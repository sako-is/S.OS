#pragma once

#include <defines.h>

#define ROUND_FLOOR(a, b) ((a) & ~((b)-1))
#define SIZEOF_ARRAY(ARRAY) (sizeof(ARRAY) / sizeof(ARRAY[0]))

#define MAX(A, B) ({ 				\
    __auto_type MAX_a = A;			\
    __auto_type MAX_b = B;			\
    MAX_a > MAX_b ? MAX_a : MAX_b;	\
})

#define DIV_ROUNDUP(VALUE, DIV) ({ \
    __auto_type DIV_ROUNDUP_value = VALUE; \
    __auto_type DIV_ROUNDUP_div = DIV; \
    (DIV_ROUNDUP_value + (DIV_ROUNDUP_div - 1)) / DIV_ROUNDUP_div; \
})

#define ALIGN_UP(VALUE, ALIGN) ({ \
    __auto_type ALIGN_UP_value = VALUE; \
    __auto_type ALIGN_UP_align = ALIGN; \
    DIV_ROUNDUP(ALIGN_UP_value, ALIGN_UP_align) * ALIGN_UP_align; \
})

#define ALIGN_DOWN(VALUE, ALIGN) ({ \
    __auto_type ALIGN_DOWN_value = VALUE; \
    __auto_type ALIGN_DOWN_align = ALIGN; \
    (ALIGN_DOWN_value / ALIGN_DOWN_align) * ALIGN_DOWN_align; \
})

size_t strlen(const char* str);
char* itoa(int num, char* buffer, int base);
char* uitoa(uint32 num, char* buffer, int base);

uint64 uRound(float64 x);
