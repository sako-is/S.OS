#pragma once

#include <defines.h>

extern char _binary_assets_Tamsyn8x16r_psf_start;
extern char _binary_assets_Tamsyn8x16r_psf_end;

typedef struct {
    uint32 magic;         /* magic bytes to identify PSF */
    uint32 version;       /* zero */
    uint32 headersize;    /* offset of bitmaps in file, 32 */
    uint32 flags;         /* 0 if there's no unicode table */
    uint32 numglyph;      /* number of glyphs */
    uint32 bytesperglyph; /* size of each glyph */
    uint32 height;        /* height in pixels */
    uint32 width;         /* width in pixels */
} PSFfont;

void putPixel(uint32 x, uint32 y, int color);

void printRectangle(int width, int height, uint32 x, uint32 y, int color);

void fillScreen(int color);

void printChar(char c, uint32 x, uint32 y, int color);
void printInt(int intg, int base, uint32 x, uint32 y, int color);
void printStr(const char* str, uint32 x, uint32 y, int color);

void printCharW(unsigned char c, uint32 x, uint32 y, int color);
// void printChar90(unsigned char c, uint32 x, uint32 y, int color);