#include <kernel/malloc.h>
#include <kernel/print/print.h>
#include <kernel/util/memory.h>

#include <limits.h>
#include <defines.h>

/*
 * Defines for often-used integral values
 * related to our binning and paging strategy.
 */
#if defined(__x86_64__) || defined(__aarch64__)
#define NUM_BINS 10U								/* Number of bins, total, under 64-bit. */
#define SMALLEST_BIN_LOG 3U							/* Logarithm base two of the smallest bin: log_2(sizeof(int64)). */
#else
#define NUM_BINS 11U								/* Number of bins, total, under 32-bit. */
#define SMALLEST_BIN_LOG 2U							/* Logarithm base two of the smallest bin: log_2(sizeof(int32)). */
#endif
#define BIG_BIN (NUM_BINS - 1)						/* Index for the big bin, (NUM_BINS - 1) */
#define SMALLEST_BIN (1UL << SMALLEST_BIN_LOG)		/* Size of the smallest bin. */

#define PAGE_SIZE 0x1000							/* Size of a page (in bytes), should be 4KB */
#define PAGE_MASK (PAGE_SIZE - 1)					/* Block mask, size of a page * number of pages - 1. */
#define SKIP_P INT32_MAX							/* INT32_MAX is half of UINT32_MAX; this gives us a 50% marker for skip lists. */
#define SKIP_MAX_LEVEL 6							/* We have a maximum of 6 levels in our skip lists. */

#define BIN_MAGIC 0xDEFAD00D

#if 1
#define assert(statement) ((statement) ? (void)0 : __assert_fail(__FILE__, __LINE__, #statement))
#else
#define assert(statement) (void)0
#endif

static void __assert_fail(const char * f, int l, const char * stmt) {
	printk("assertion failed in {#ff0000.s}:{d} {#ffffff.s}\n", f, l, stmt);
}

void* malloc(size_t size);
void* realloc(void* ptr, size_t size);
void* calloc(size_t nmemb, size_t size);
void* valloc(size_t size);
void free(void* ptr);

static inline size_t malloc_adjust_bin(size_t bin) {
	if (bin <= (size_t)SMALLEST_BIN_LOG) return 0;
	bin -= SMALLEST_BIN_LOG + 1;
	
	if (bin > (size_t)BIG_BIN) return BIG_BIN;

	return bin;
}

/*
 * Given a size value, find the correct bin
 * to place the requested allocation in.
 */
static inline size_t malloc_bin_size(size_t size) {
	size_t bin = sizeof(size) * CHAR_BIT - __builtin_clzl(size);
	bin += !!(size & (size - 1));
	return malloc_adjust_bin(bin);
}

/*
 * Bin header - One page of memory.
 * Appears at the front of a bin to point to the
 * previous bin (or NULL if the first), the next bin
 * (or NULL if the last) and the head of the bin, which
 * is a stack of cells of data.
 */
typedef struct _malloc_bin_header {
	struct _malloc_bin_header *  next;	/* Pointer to the next node. */
	void * head;							/* Head of this bin. */
	size_t size;							/* Size of this bin, if big; otherwise bin index. */
	size_t bin_magic;
} malloc_bin_header;

/*
 * A big bin header is basically the same as a regular bin header
 * only with a pointer to the previous (physically) instead of
 * a "next" and with a list of forward headers.
 */
typedef struct _malloc_big_bin_header {
	struct _malloc_big_bin_header * next;
	void * head;
	size_t size;
	size_t bin_magic;
	struct _malloc_big_bin_header * prev;
	struct _malloc_big_bin_header * forward[SKIP_MAX_LEVEL+1];
} malloc_big_bin_header;


/*
 * List of pages in a bin.
 */
typedef struct _malloc_bin_header_head {
	malloc_bin_header * first;
} malloc_bin_header_head;

/*
 * Array of available bins.
 */
static malloc_bin_header_head malloc_bin_head[NUM_BINS - 1];	/* Small bins */
static struct _malloc_big_bins {
	malloc_big_bin_header head;
	int level;
} malloc_big_bins;
static malloc_big_bin_header * malloc_newest_big = NULL;		/* Newest big bin */

/* }}} Bin management */
/* Doubly-Linked List {{{ */

/*
 * Remove an entry from a page list.
 * Decouples the element from its
 * position in the list by linking
 * its neighbors to eachother.
 */
static inline void malloc_list_decouple(malloc_bin_header_head *head, malloc_bin_header *node) {
	malloc_bin_header *next	= node->next;
	head->first = next;
	node->next = NULL;
}

/*
 * Insert an entry into a page list.
 * The new entry is placed at the front
 * of the list and the existing border
 * elements are updated to point back
 * to it (our list is doubly linked).
 */
static inline void malloc_list_insert(malloc_bin_header_head *head, malloc_bin_header *node) {
	node->next = head->first;
	head->first = node;
}

/*
 * Get the head of a page list.
 * Because redundant function calls
 * are really great, and just in case
 * we change the list implementation.
 */
static inline malloc_bin_header* malloc_list_head(malloc_bin_header_head *head) {
	return head->first;
}

/* }}} Lists */
/* Skip List {{{ */

/*
 * Skip lists are efficient
 * data structures for storing
 * and searching ordered data.
 *
 * Here, the skip lists are used
 * to keep track of big bins.
 */

/*
 * Generate a random value in an appropriate range.
 * This is a xor-shift RNG.
 */
static uint32_t malloc_skip_rand(void) {
	static uint32_t x = 123456789;
	static uint32_t y = 362436069;
	static uint32_t z = 521288629;
	static uint32_t w = 88675123;

	uint32_t t;

	t = x ^ (x << 11);
	x = y; y = z; z = w;
	return w = w ^ (w >> 19) ^ t ^ (t >> 8);
}

/*
 * Generate a random level for a skip node
 */
static inline int malloc_random_level(void) {
	int level = 0;
	/*
	 * Keep trying to check rand() against 50% of its maximum.
	 * This provides 50%, 25%, 12.5%, etc. chance for each level.
	 */
	while (malloc_skip_rand() < SKIP_P && level < SKIP_MAX_LEVEL) ++level;
	return level;
}

/*
 * Find best fit for a given value.
 */
static malloc_big_bin_header* malloc_skip_list_findbest(size_t search_size) {
	malloc_big_bin_header* node = &malloc_big_bins.head;
	/*
	 * Loop through the skip list until we hit something > our search value.
	 */
	for (int i = malloc_big_bins.level; i >= 0; --i) {
		while (node->forward[i] && (node->forward[i]->size < search_size)) {
			node = node->forward[i];
			if (node)
				assert((node->size + sizeof(malloc_big_bin_header)) % PAGE_SIZE == 0);
		}
	}
	/*
	 * This value will either be NULL (we found nothing)
	 * or a node (we found a minimum fit).
	 */
	node = node->forward[0];
	if (node) {
		assert((size_t)node % PAGE_SIZE == 0);
		assert((node->size + sizeof(malloc_big_bin_header)) % PAGE_SIZE == 0);
	}
	return node;
}

/*
 * Insert a header into the skip list.
 */
static void malloc_skip_list_insert(malloc_big_bin_header * value) {
	/*
	 * You better be giving me something valid to insert,
	 * or I will slit your ****ing throat.
	 */
	assert(value != NULL);
	assert(value->head != NULL);
	assert((size_t)value->head > (size_t)value);
	if (value->size > NUM_BINS) {
		assert((size_t)value->head < (size_t)value + value->size);
	} else {
		assert((size_t)value->head < (size_t)value + PAGE_SIZE);
	}
	assert((size_t)value % PAGE_SIZE == 0);
	assert((value->size + sizeof(malloc_big_bin_header)) % PAGE_SIZE == 0);
	assert(value->size != 0);

	/*
	 * Starting from the head node of the bin locator...
	 */
	malloc_big_bin_header * node = &malloc_big_bins.head;
	malloc_big_bin_header * update[SKIP_MAX_LEVEL + 1];

	/*
	 * Loop through the skiplist to find the right place
	 * to insert the node (where ->forward[] > value)
	 */
	int i;
	for (i = malloc_big_bins.level; i >= 0; --i) {
		while (node->forward[i] && node->forward[i]->size < value->size) {
			node = node->forward[i];
			if (node)
				assert((node->size + sizeof(malloc_big_bin_header)) % PAGE_SIZE == 0);
		}
		update[i] = node;
	}
	node = node->forward[0];

	/*
	 * Make the new skip node and update
	 * the forward values.
	 */
	if (node != value) {
		int level = malloc_random_level();
		/*
		 * Get all of the nodes before this.
		 */
		if (level > malloc_big_bins.level) {
			for (i = malloc_big_bins.level + 1; i <= level; ++i) {
				update[i] = &malloc_big_bins.head;
			}
			malloc_big_bins.level = level;
		}

		/*
		 * Make the new node.
		 */
		node = value;

		/*
		 * Run through and point the preceeding nodes
		 * for each level to the new node.
		 */
		for (i = 0; i <= level; ++i) {
			node->forward[i] = update[i]->forward[i];
			if (node->forward[i])
				assert((node->forward[i]->size + sizeof(malloc_big_bin_header)) % PAGE_SIZE == 0);
			update[i]->forward[i] = node;
		}
	}
}

/*
 * Delete a header from the skip list.
 * Be sure you didn't change the size, or we won't be able to find it.
 */
static void malloc_skip_list_delete(malloc_big_bin_header * value) {
	/*
	 * Debug assertions
	 */
	assert(value != NULL);
	assert(value->head);
	assert((size_t)value->head > (size_t)value);
	if (value->size > NUM_BINS) assert((size_t)value->head < (size_t)value + value->size);
	else assert((size_t)value->head < (size_t)value + PAGE_SIZE);

	/*
	 * Starting from the bin header, again...
	 */
	malloc_big_bin_header * node = &malloc_big_bins.head;
	malloc_big_bin_header * update[SKIP_MAX_LEVEL + 1];

	/*
	 * Find the node.
	 */
	int i;
	for (i = malloc_big_bins.level; i >= 0; --i) {
		while (node->forward[i] && node->forward[i]->size < value->size) {
			node = node->forward[i];
			if (node) assert((node->size + sizeof(malloc_big_bin_header)) % PAGE_SIZE == 0);
		}
		update[i] = node;
	}
	node = node->forward[0];
	while (node != value) node = node->forward[0];

	if (node != value) {
		node = malloc_big_bins.head.forward[0];
		while (node->forward[0] && node->forward[0] != value) node = node->forward[0];
		node = node->forward[0];
	}
	/*
	 * If we found the node, delete it;
	 * otherwise, we do nothing.
	 */
	if (node == value) {
		for (i = 0; i <= malloc_big_bins.level; ++i) {
			if (update[i]->forward[i] != node) break;
			update[i]->forward[i] = node->forward[i];
			if (update[i]->forward[i]) {
				assert((size_t)(update[i]->forward[i]) % PAGE_SIZE == 0);
				assert((update[i]->forward[i]->size + sizeof(malloc_big_bin_header)) % PAGE_SIZE == 0);
			}
		}

		while (malloc_big_bins.level > 0 && malloc_big_bins.head.forward[malloc_big_bins.level] == NULL) --malloc_big_bins.level;
	}
}

/*
 * Pop an item from a block.
 * Free space is stored as a stack,
 * so we get a free space for a bin
 * by popping a free node from the
 * top of the stack.
 */
static void * malloc_stack_pop(malloc_bin_header *header) {
	assert(header);
	assert(header->head != NULL);
	assert((size_t)header->head > (size_t)header);
	if (header->size > NUM_BINS) assert((size_t)header->head < (size_t)header + header->size);
	else {
		assert((size_t)header->head < (size_t)header + PAGE_SIZE);
		assert((size_t)header->head > (size_t)header + sizeof(malloc_bin_header) - 1);
	}
	
	/*
	 * Remove the current head and point
	 * the head to where the old head pointed.
	 */
	void *item = header->head;
	size_t **head = header->head;
	size_t *next = *head;
	header->head = next;
	return item;
}

/*
 * Push an item into a block.
 * When we free memory, we need
 * to add the freed cell back
 * into the stack of free spaces
 * for the block.
 */
static void malloc_stack_push(malloc_bin_header *header, void *ptr) {
	assert(ptr != NULL);
	assert((size_t)ptr > (size_t)header);
	if (header->size > NUM_BINS) assert((size_t)ptr < (size_t)header + header->size);
	else {
		assert((((size_t)ptr - sizeof(malloc_bin_header)) & ((1UL << (header->size + SMALLEST_BIN_LOG)) - 1)) == 0);
		assert((size_t)ptr < (size_t)header + PAGE_SIZE);
	}
	size_t **item = (size_t **)ptr;
	*item = (size_t *)header->head;
	header->head = item;
}

/*
 * Is this cell stack empty?
 * If the head of the stack points
 * to NULL, we have exhausted the
 * stack, so there is no more free
 * space available in the block.
 */
static inline int malloc_stack_empty(malloc_bin_header *header) {
	return header->head == NULL;
}

void* malloc(size_t size) {
	/*
	 * C standard implementation:
	 * If size is zero, we can choose do a number of things.
	 * This implementation will return a NULL pointer.
	 */
	if (__builtin_expect(size == 0, 0)) return NULL;
	/*
	 * Find the appropriate bin for the requested
	 * allocation and start looking through that list.
	 */
	unsigned int bucket_id = malloc_bin_size(size);

	if (bucket_id < BIG_BIN) {
		/*
		 * Small bins.
		 */
		malloc_bin_header * bin_header = malloc_list_head(&malloc_bin_head[bucket_id]);
		if (!bin_header) {
			/*
			 * Grow the heap for the new bin.
			 */
			bin_header = (malloc_bin_header*)sbrk(PAGE_SIZE);
			bin_header->bin_magic = BIN_MAGIC;
			// assert((size_t)bin_header % PAGE_SIZE == 0);

			/*
			 * Set the head of the stack.
			 */
			bin_header->head = (void*)((size_t)bin_header + sizeof(malloc_bin_header));
			/*
			 * Insert the new bin at the front of
			 * the list of bins for this size.
			 */
			malloc_list_insert(&malloc_bin_head[bucket_id], bin_header);
			/*
			 * Initialize the stack inside the bin.
			 * The stack is initially full, with each
			 * entry pointing to the next until the end
			 * which points to NULL.
			 */
			size_t adj = SMALLEST_BIN_LOG + bucket_id;
			size_t i, available = ((PAGE_SIZE - sizeof(malloc_bin_header)) >> adj) - 1;

			size_t **base = bin_header->head;
			for (i = 0; i < available; ++i) {
				/*
				 * Our available memory is made into a stack, with each
				 * piece of memory turned into a pointer to the next
				 * available piece. When we want to get a new piece
				 * of memory from this block, we just pop off a free
				 * spot and give its address.
				 */
				base[i << bucket_id] = (size_t *)&base[(i + 1) << bucket_id];
			}
			base[available << bucket_id] = NULL;
			bin_header->size = bucket_id;
		} else assert(bin_header->bin_magic == BIN_MAGIC);

		size_t ** item = malloc_stack_pop(bin_header);
		if (malloc_stack_empty(bin_header)) malloc_list_decouple(&(malloc_bin_head[bucket_id]),bin_header);

		return item;
	} else {
		/*
		 * Big bins.
		 */
		malloc_big_bin_header * bin_header = malloc_skip_list_findbest(size);
		if (bin_header) {
			assert(bin_header->size >= size);
			/*
			 * If we found one, delete it from the skip list
			 */
			malloc_skip_list_delete(bin_header);
			/*
			 * Retreive the head of the block.
			 */
			size_t ** item = malloc_stack_pop((malloc_bin_header *)bin_header);
			return item;
		} else {
			/*
			 * Round requested size to a set of pages, plus the header size.
			 */
			size_t pages = (size + sizeof(malloc_big_bin_header)) / PAGE_SIZE + 1;
			bin_header = (malloc_big_bin_header*)sbrk(PAGE_SIZE * pages);
			bin_header->bin_magic = BIN_MAGIC;
			assert((size_t)bin_header % PAGE_SIZE == 0);
			/*
			 * Give the header the remaining space.
			 */
			bin_header->size = pages * PAGE_SIZE - sizeof(malloc_big_bin_header);
			assert((bin_header->size + sizeof(malloc_big_bin_header)) % PAGE_SIZE == 0);
			/*
			 * Link the block in physical memory.
			 */
			bin_header->prev = malloc_newest_big;
			if (bin_header->prev) bin_header->prev->next = bin_header;

			malloc_newest_big = bin_header;
			bin_header->next = NULL;
			/*
			 * Return the head of the block.
			 */
			bin_header->head = NULL;
			return (void*)((size_t)bin_header + sizeof(malloc_big_bin_header));
		}
	}
}

void free(void *ptr) {
	/*
	 * C standard implementation: Do nothing when NULL is passed to free.
	 */
	if (__builtin_expect(ptr == NULL, 0)) return;

	/*
	 * Woah, woah, hold on, was this a page-aligned block?
	 */
	if ((size_t)ptr % PAGE_SIZE == 0) {
		/*
		 * Well howdy-do, it was.
		 */
		ptr = (void *)((size_t)ptr - 1);
	}

	/*
	 * Get our pointer to the head of this block by
	 * page aligning it.
	 */
	malloc_bin_header * header = (malloc_bin_header *)((size_t)ptr & (size_t)~PAGE_MASK);
	assert((size_t)header % PAGE_SIZE == 0);

	if (header->bin_magic != BIN_MAGIC)
		return;

	/*
	 * For small bins, the bin number is stored in the size
	 * field of the header. For large bins, the actual size
	 * available in the bin is stored in this field. It's
	 * easy to tell which is which, though.
	 */
	size_t bucket_id = header->size;
	if (bucket_id > (size_t)NUM_BINS) {
		bucket_id = BIG_BIN;
		malloc_big_bin_header *bheader = (malloc_big_bin_header*)header;
		
		assert(bheader);
		assert(bheader->head == NULL);
		assert((bheader->size + sizeof(malloc_big_bin_header)) % PAGE_SIZE == 0);

		/*
		 * Push new space back into the stack.
		 */
		malloc_stack_push((malloc_bin_header *)bheader, (void *)((size_t)bheader + sizeof(malloc_big_bin_header)));
		assert(bheader->head != NULL);
		/*
		 * Insert the block into list of available slabs.
		 */
		malloc_skip_list_insert(bheader);
	} else {

		/*
		 * If the stack is empty, we are freeing
		 * a block from a previously full bin.
		 * Return it to the busy bins list.
		 */
		if (malloc_stack_empty(header)) malloc_list_insert(&malloc_bin_head[bucket_id], header);
		/*
		 * Push new space back into the stack.
		 */
		malloc_stack_push(header, ptr);
	}
}

void* valloc(size_t size) {
	/*
	 * Allocate a page-aligned block.
	 * XXX: THIS IS HORRIBLY, HORRIBLY WASTEFUL!! ONLY USE THIS
	 *	  IF YOU KNOW WHAT YOU ARE DOING!
	 */
	size_t true_size = size + PAGE_SIZE - sizeof(malloc_big_bin_header); /* Here we go... */
	void * result = malloc(true_size);
	void * out = (void *)((size_t)result + (PAGE_SIZE - sizeof(malloc_big_bin_header)));
	assert((size_t)out % PAGE_SIZE == 0);
	return out;
}

void* realloc(void *ptr, size_t size) {
	/*
	 * C standard implementation: When NULL is passed to realloc,
	 * simply malloc the requested size and return a pointer to that.
	 */
	if (__builtin_expect(ptr == NULL, 0)) return malloc(size);

	/*
	 * C standard implementation: For a size of zero, free the
	 * pointer and return NULL, allocating no new memory.
	 */
	if (__builtin_expect(size == 0, 0)) {
		free(ptr);
		return NULL;
	}

	/*
	 * Find the bin for the given pointer
	 * by aligning it to a page.
	 */
	malloc_bin_header * header_old = (void *)((size_t)ptr & (size_t)~PAGE_MASK);
	if (header_old->bin_magic != BIN_MAGIC) {
		assert(0 && "Bad magic on realloc.");
		return NULL;
	}

	size_t old_size = header_old->size;
	if (old_size < (size_t)BIG_BIN) {
		/*
		 * If we are copying from a small bin,
		 * we need to get the size of the bin
		 * from its id.
		 */
		old_size = (1UL << (SMALLEST_BIN_LOG + old_size));
	}

	/*
	 * (This will only happen for a big bin, mathematically speaking)
	 * If we still have room in our bin for the additonal space,
	 * we don't need to do anything.
	 */
	if (old_size >= size) return ptr;

	/*
	 * Reallocate more memory.
	 */
	void * newptr = malloc(size);
	if (__builtin_expect(newptr != NULL, 1)) {

		/*
		 * Copy the old value into the new value.
		 * Be sure to only copy as much as was in
		 * the old block.
		 */
		memcpy(newptr, ptr, old_size);
		free(ptr);
		return newptr;
	}

	/*
	 * We failed to allocate more memory,
	 * which means we're probably out.
	 *
	 * Bail and return NULL.
	 */
	return NULL;
}

void* calloc(size_t nmemb, size_t size) {
	/*
	 * Allocate memory and zero it before returning
	 * a pointer to the newly allocated memory.
	 * 
	 * Implemented by way of a simple malloc followed
	 * by a memset to 0x00 across the length of the
	 * requested memory chunk.
	 */

	void *ptr = malloc(nmemb * size);
	if (__builtin_expect(ptr != NULL, 1)) memset(ptr,0x00,nmemb * size);
	return ptr;
}
