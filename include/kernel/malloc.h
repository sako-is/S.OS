#pragma once

#include <defines.h>

/*
* TODO: Order of implementation (starting relatively simple)
* Linked list heap
* SLAB heap https://en.wikipedia.org/wiki/Slab_allocation
* SLUB heap?
*/

void* malloc(size_t size);
void* realloc(void* ptr, size_t size);
void* calloc(size_t nmemb, size_t size);
void* valloc(size_t size);
void free(void* ptr);
