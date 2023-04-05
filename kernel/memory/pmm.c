#include <limine/limine.h>
#include <kernel/memory/pmm.h>
#include <kernel/memory/vmm.h>
#include <kernel/print/print.h>
#include <kernel/util/memory.h>
#include <kernel/util/util.h>
#include <structures/bitmap.h>

#include <defines.h>

volatile struct limine_memmap_request memmap_request = {
	.id = LIMINE_MEMMAP_REQUEST,
	.revision = 0
};

static uint8* bitmap = NULL;
static uint64 highest_page_index = 0;
static uint64 last_used_index = 0;
static uint64 usable_pages = 0;
static uint64 used_pages = 0;
static uint64 reserved_pages = 0;

void initPMM(void) {
	struct limine_memmap_response *memmap = memmap_request.response;
	struct limine_hhdm_response *hhdm = hhdm_request.response;
	struct limine_memmap_entry **entries = memmap->entries;

	uint64 highest_addr = 0;

	// Calculate how big the memory map needs to be.
	for (size_t i = 0; i < memmap->entry_count; i++) {
		struct limine_memmap_entry* entry = entries[i];

		// printk("{#00ffff}PMM: {#ffffff}Memory map entry: base={X}, length={X}, type={X}\n", entry->base, entry->length, entry->type);

		switch (entry->type) {
			case LIMINE_MEMMAP_USABLE:
				usable_pages += DIV_ROUNDUP(entry->length, PAGE_SIZE);
				highest_addr = MAX(highest_addr, entry->base + entry->length);
				break;
			case LIMINE_MEMMAP_RESERVED:
			case LIMINE_MEMMAP_ACPI_RECLAIMABLE:
			case LIMINE_MEMMAP_ACPI_NVS:
			case LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE:
			case LIMINE_MEMMAP_KERNEL_AND_MODULES:
				reserved_pages += DIV_ROUNDUP(entry->length, PAGE_SIZE);
				break;
		}
	}

	// Calculate the needed size for the bitmap in bytes and align it to page size.
	highest_page_index = highest_addr / PAGE_SIZE;
	uint64 bitmap_size = ALIGN_UP(highest_page_index / 8, PAGE_SIZE);

	printk("{#00ffff}PMM: {#ffffff}Highest address: {X}\n", highest_addr);
	printk("{#00ffff}PMM: {#ffffff}Bitmap size: {u} bytes\n", bitmap_size);

	// Find a hole for the bitmap in the memory map.
	for (size_t i = 0; i < memmap->entry_count; i++) {
		struct limine_memmap_entry *entry = entries[i];
		if (entry->type != LIMINE_MEMMAP_USABLE)
			continue;

		if (entry->length >= bitmap_size) {
			bitmap = (uint8*)(entry->base + hhdm->offset);
			
			// Initialise entire bitmap to 1 (non-free)
			memset(bitmap, 0xff, bitmap_size);

			entry->length -= bitmap_size;
			entry->base += bitmap_size;

			break;
		}
	}
	
	// Populate free bitmap entries according to the memory map.
	for (size_t i = 0; i < memmap->entry_count; i++) {
		struct limine_memmap_entry *entry = entries[i];

		if (entry->type != LIMINE_MEMMAP_USABLE) continue;

		for (uint64 j = 0; j < entry->length; j += PAGE_SIZE) resetBitmap(bitmap, (entry->base + j) / PAGE_SIZE);
	}

	printk("{#00ffff}PMM: {#ffffff}Usable memory: {u}MiB\n", (usable_pages * 4096) / 1024 / 1024);
	printk("{#00ffff}PMM: {#ffffff}Reserved memory: {u}MiB\n", (reserved_pages * 4096) / 1024 / 1024);
}

static void* innerAlloc(size_t pages, uint64 limit) {
	size_t p = 0;
	size_t page = last_used_index - pages;
   
	while(last_used_index < limit) {
		if (!testBitmap(bitmap, last_used_index++)) {
			if (++p == pages) {
				for (size_t i = page; i < last_used_index; i++) { 
					setBitmap(bitmap, i);
				}
				return (void*)(page * PAGE_SIZE);
			}
		} else {
			p = 0;
		}
	}

	return NULL;
}

void* PMMalloc(size_t pages) {
	void* ret = PMMallocNoZero(pages);
	if (ret != NULL) {
		memset(ret + VMM_HIGHER_HALF, 0, pages * PAGE_SIZE);
	}

	return ret;
}

void* PMMallocNoZero(size_t pages) {
	// printk("{#0000ff}	Highest Page Index: {u}\n", highest_page_index);
	// if(last_used_index == 0) printk("{#0000ff}	Last Used Index: 0\n");
	// else printk("{#0000ff}	Last Used Index: {d}\n", last_used_index);

	size_t last = last_used_index;
	void *ret = innerAlloc(pages, highest_page_index);

	if (ret == NULL) {
		last_used_index = 0;
		ret = innerAlloc(pages, last);
	}
	
	used_pages += pages;
	return ret;
}

void PMMfree(void *addr, size_t pages) {
	size_t page = (uint64)addr / PAGE_SIZE;
	for (size_t i = page; i < page + pages; i++) {
		resetBitmap(bitmap, i);
	}
	used_pages -= pages;
}
