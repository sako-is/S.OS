#include <kernel/util/memory.h>

#include <defines.h>

#define MEMORY_CAPACITY 20000

void memcpy(void *dest, void *src, size_t n) { 
	char* csrc = (char*)src; 
	char* cdest = (char*)dest; 

	for(int i = 0; i < (int)n; i++) cdest[i] = csrc[i]; 
} 

void memzero(void * s, uint64 n) {
	for(int i = 0; i < (int)n; i++) ((uint8*)s)[i] = 0;
}

void* memset(void *b, int c, int len) {
	char *b_char = (char *)b;

    if (b == NULL) return NULL;

    while(*b_char && len > 0) {
        *b_char = c;
        b_char++;
        len--;
    }

    return b;
}

void* sbrk(int increment) {
	static char global_mem[MEMORY_CAPACITY] = {0};
	static char *p_break = global_mem;

	char* const limit = global_mem + MEMORY_CAPACITY;
	char* const original = p_break;

	if(increment < global_mem - p_break  ||  increment >= limit - p_break) return (void*)-1;
	
	p_break += increment;

	return original;
}
