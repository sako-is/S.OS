#pragma once

#include <defines.h>

/*
* TODO: Order of implementation (starting relatively simple)
* Linked list heap
* SLAB heap https://en.wikipedia.org/wiki/Slab_allocation
* SLUB heap?
*/

void* mallock(size_t size);
void free(void* ptr);