#include <kernel/malloc.h>
#include <kernel/util/util.h>
#include <kernel/util/memory.h>
#include <kernel/memory/pmm.h>
#include <kernel/memory/vmm.h>
#include <kernel/print/print.h>
#include <limine/limine.h>

#include <defines.h>

volatile struct limine_hhdm_request hhdm_request = {
	.id = LIMINE_HHDM_REQUEST,
	.revision = 0
};

typedef struct Slab {
	void** first;
	size_t size;
} Slab;

typedef struct SlabHeader {
	Slab* slab;
} SlabHeader;

typedef struct AllocMetadata {
	size_t pages;
	size_t size;
} AllocMetadata;

static Slab slabs[10];

static inline Slab* slabFor(size_t size) {
	for (size_t i = 0; i < SIZEOF_ARRAY(slabs); i++) {
		Slab *slab = &slabs[i];
		if (slab->size >= size) return slab;
	}
	return NULL;
}

static void createSlab(Slab *slab, size_t size) {
	slab->first = PMMallocNoZero(1) + VMM_HIGHER_HALF;
	slab->size = size;

	size_t header_offset = ALIGN_UP(sizeof(SlabHeader), size);
	size_t available_size = PAGE_SIZE - header_offset;

	slab->first = (void**)((void*)slab->first + header_offset);

	void** arr = (void**)slab->first;
	size_t max = available_size / size - 1;
	size_t fact = size / sizeof(Slab);

	for (size_t i = 1; i < max; i++) {
		arr[(i - 1) * fact] = arr[(i) * fact];
	}
	arr[max * fact] = NULL;
}

static void *allocFromSlab(Slab *slab) {
	if (slab->first == NULL) createSlab(slab, slab->size);

	void **old_free = slab->first;
	slab->first = *old_free;
	memset(old_free, 0, slab->size);

	return old_free;
}

static void freeInSlab(Slab *slab, void *ptr) {
	if (ptr == NULL) return; 

	void** new_head = ptr;
	*new_head = slab->first;
	slab->first = new_head;

}

void initMalloc(void) {
	createSlab(&slabs[0], 8);
	createSlab(&slabs[1], 16);
	createSlab(&slabs[2], 24);
	createSlab(&slabs[3], 32);
	createSlab(&slabs[4], 48);
	createSlab(&slabs[5], 64);
	createSlab(&slabs[6], 128);
	createSlab(&slabs[7], 256);
	createSlab(&slabs[8], 512);
	createSlab(&slabs[9], 1024);
}

void *malloc(size_t size) {
	Slab *slab = slabFor(size);
	if (slab != NULL) {
		return allocFromSlab(slab);
	}

	size_t page_count = DIV_ROUNDUP(size, PAGE_SIZE);
	void *ret = PMMalloc(page_count + 1);
	if (ret == NULL) return NULL;

	ret += VMM_HIGHER_HALF;
	AllocMetadata *metadata = (AllocMetadata *)ret;

	metadata->pages = page_count;
	metadata->size = size;

	return ret + PAGE_SIZE;
}

void *realloc(void *ptr, size_t new_size) {
	if (ptr == NULL) {
		return malloc(new_size);
	}

	if (((uintptr_t)ptr & 0xfff) == 0) {
		AllocMetadata *metadata = (AllocMetadata *)(ptr - PAGE_SIZE);
		if (DIV_ROUNDUP(metadata->size, PAGE_SIZE) == DIV_ROUNDUP(new_size, PAGE_SIZE)) {
			metadata->size = new_size;
			return ptr;
		}

		void *new_ptr = malloc(new_size);
		if (new_ptr == NULL) {
			return NULL;
		}

		if (metadata->size > new_size) {
			memcpy(new_ptr, ptr, new_size);
		} else {
			memcpy(new_ptr, ptr, metadata->size);
		}

		free(ptr);
		return new_ptr;
	}

	SlabHeader *slab_header = (SlabHeader *)((uintptr_t)ptr & ~0xfff);
	Slab *slab = slab_header->slab;

	if (new_size > slab->size) {
		void *new_ptr = malloc(new_size);
		if (new_ptr == NULL) {
			return NULL;
		}

		memcpy(new_ptr, ptr, slab->size);
		freeInSlab(slab, ptr);
		return new_ptr;
	}

	return ptr;
}

void free(void *ptr) {
	if (ptr == NULL) {
		return;
	}

	if (((uintptr_t)ptr & 0xfff) == 0) {
		AllocMetadata *metadata = (AllocMetadata *)(ptr - PAGE_SIZE);
		PMMfree((void *)metadata - VMM_HIGHER_HALF, metadata->pages + 1);
		return;
	}

	SlabHeader *slab_header = (SlabHeader *)((uintptr_t)ptr & ~0xfff);
	freeInSlab(slab_header->slab, ptr);
}
