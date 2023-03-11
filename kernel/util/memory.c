#include <kernel/util/memory.h>

#include <defines.h>

void memcpy(void *dest, void *src, size_t n) { 
	char* csrc = (char*)src; 
	char* cdest = (char*)dest; 

	for(int i = 0; i < (int)n; i++) cdest[i] = csrc[i]; 
} 

void memzero(void * s, uint64 n) {
	for(int i = 0; i < (int)n; i++) ((uint8*)s)[i] = 0;
}