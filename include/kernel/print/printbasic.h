#pragma once

#include <defines.h>

#define white     0xffffff
#define black     0x000000
#define red       0xff0000
#define green     0x00ff00
#define blue      0x0000ff
#define darkred   
#define darkgreen 
#define darkblue  0x000219
#define teal      0x008080

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

void tPrintChar(char c, uint32 x, uint32 y, int color);
void tPrintInt(int intg, int base, uint32 x, uint32 y, int color);
void tPrintStr(const char* str, uint32 x, uint32 y, int color);

void tPrintCharW(unsigned char c, uint32 x, uint32 y, int color);
// void printChar90(unsigned char c, uint32 x, uint32 y, int color);