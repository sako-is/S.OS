#pragma once

#include <defines.h>

typedef struct Bitmap {
	uint8* data;
	size_t size;
} Bitmap;

static inline bool testBitmap(void *bitmap, size_t bit) {
    uint8_t *bitmap_u8 = bitmap;
    return bitmap_u8[bit / 8] & (1 << (bit % 8));
}

static inline void setBitmap(void *bitmap, size_t bit) {
    uint8_t *bitmap_u8 = bitmap;
    bitmap_u8[bit / 8] |= (1 << (bit % 8));
}

static inline void resetBitmap(void *bitmap, size_t bit) {
    uint8_t *bitmap_u8 = bitmap;
    bitmap_u8[bit / 8] &= ~(1 << (bit % 8));
}
