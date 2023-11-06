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
	char *bchar = (char *)b;

    if (b == NULL) 
        return NULL;

    while(*bchar && len > 0) {
        *bchar = c;
        bchar++;
        len--;
    }

    return b;
}

void* sbrk(int increment) {
	static char global_mem[MEMORY_CAPACITY] = {0};
	static char *pbreak = global_mem;

	char* const limit = global_mem + MEMORY_CAPACITY;
	char* const original = pbreak;

	if(increment < global_mem - pbreak  ||  increment >= limit - pbreak) 
        return (void*)-1;
	
	pbreak += increment;

	return original;
}