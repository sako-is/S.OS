#pragma once

#include <defines.h>

/*
* malloc from https://github.com/klange/toaruos/blob/master/kernel/misc/malloc.c
*/
void initMalloc(void);

void* malloc(size_t size);
void* realloc(void* ptr, size_t size);
void free(void* ptr);

