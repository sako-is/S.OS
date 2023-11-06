#include <kernel/print/printbasic.h>
#include <kernel/kernel.h>
#include <kernel/print/print.h>
#include <kernel/util/util.h>

#include <defines.h>

void putPixel(uint32 x, uint32 y, int color) {
	uint32 where = x*fb_request.response->framebuffers[0]->bpp/8 + y*fb_request.response->framebuffers[0]->pitch; // *fb_request.response->framebuffers[0]->bpp
	unsigned char* screen = FB_ADDRESS;
	screen[where]     = color & 255;          // blue
	screen[where + 1] = (color >> 8) & 255;   // green
	screen[where + 2] = (color >> 16) & 255;  // red
}

void printRectangle(int width, int height, uint32 x, uint32 y, int color) {
	uint16 bpp = fb_request.response->framebuffers[0]->bpp;
	uint64 pitch = fb_request.response->framebuffers[0]->pitch;
	uint32 where = x*fb_request.response->framebuffers[0]->bpp/8 + y*fb_request.response->framebuffers[0]->pitch; 

	unsigned char* screen = FB_ADDRESS;

	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			screen[where + j*bpp/8] = color & 255;
			screen[where + j*bpp/8 + 1] = (color >> 8) & 255;
			screen[where + j*bpp/8 + 2] = (color >> 16) & 255;
		}
		screen+=pitch;
	}
}

void fillScreen(int color) {
	printRectangle(fb_request.response->framebuffers[0]->height, fb_request.response->framebuffers[0]->pitch, 0, 0, color);
	term_ctx.x = 5;
	term_ctx.y = 5;
}

void tPrintChar(char c, uint32 x, uint32 y, int color) {
	uint16 bpp = fb_request.response->framebuffers[0]->bpp;
	uint64 pitch = fb_request.response->framebuffers[0]->pitch;
	uint32 where = x*fb_request.response->framebuffers[0]->bpp/8 + y*fb_request.response->framebuffers[0]->pitch; 
	unsigned char* screen = FB_ADDRESS;

	PSFfont *font = term_ctx.font;

	int mask[8] = { 1, 2, 4, 8, 16, 32, 64, 128 };
	unsigned char* glyph = font->headersize + (unsigned char*)font + (c > 0 && (uint32)c < font->numglyph ? c : 0) * font->bytesperglyph;

	for(uint32 i = 0; i < font->height; i++) {
		for(uint32 j = 0; j < font->width; j++) {
			if(glyph[i] & mask[j]) {
				screen[where + (font->width-j)*bpp/8] = color & 255;
				screen[where + (font->width-j)*bpp/8 + 1] = (color >> 8) & 255;
				screen[where + (font->width-j)*bpp/8 + 2] = (color >> 16) & 255;
			}
		}
		screen += pitch;
	}
}

void tPrintStr(const char* str, uint32 x, uint32 y, int color) {
	PSFfont* font = term_ctx.font;
	int max_chars_per_line = (fb_request.response->framebuffers[0]->width - x) / (font->width + 1);

	for (int i = 0, j = 0; i < (int)strlen(str); i++, j++) {
		if (str[i] == '\n') { 
			j = -1; i--; y += font->height + 1; 
			continue; 
		} else if (j >= max_chars_per_line) { 
			j = -1; y += font->height + 1; 
			continue; 
		}

		if(y >= fb_request.response->framebuffers[0]->height - font->height) {
			fillScreen(term_ctx.bg);
			x = term_ctx.xsetting;
			y = term_ctx.ysetting;
		}

		tPrintChar(str[i], x + (font->width + 1) * j, y, color);
	}
}

void tPrintInt(int intg, int base, uint32 x, uint32 y, int color) {
	char buffer[33];
	char* buf = itoa(intg, buffer, base);
	tPrintStr((const char*)buf, x, y, color);
}

void tPrintCharW(unsigned char c, uint32 x, uint32 y, int color) {
	uint16 bpp = fb_request.response->framebuffers[0]->bpp;
	uint64 pitch = fb_request.response->framebuffers[0]->pitch;
	uint32 where = x*fb_request.response->framebuffers[0]->bpp/8 + y*fb_request.response->framebuffers[0]->pitch; 
	unsigned char* screen = FB_ADDRESS;

	PSFfont* font = term_ctx.font;

	int mask[8] = { 1, 2, 4, 8, 16, 32, 64, 128 };
	unsigned char* glyph = font->headersize + (unsigned char*)font + (c > 0 && c < font->numglyph ? c : 0) * font->bytesperglyph;

	for(uint32 i = 0; i < font->height; i++) {
		for(uint32 j = 0; j < font->width; j++) {
			if(glyph[i] & mask[j]) {
				screen[where + j*bpp/8] = color & 255;
				screen[where + j*bpp/8 + 1] = (color >> 8) & 255;
				screen[where + j*bpp/8 + 2] = (color >> 16) & 255;
			}
		}
		screen += pitch;
	}
}

/* 
void printChar(unsigned char c, uint32 x, uint32 y, int color) {
	PSFfont *font = (PSFfont*)&_binary_assets_Tamsyn8x16r_psf_start;

	int mask[8] = { 1, 2, 4, 8, 16, 32, 64, 128 };
	unsigned char* glyph = font->headersize + (unsigned char*)font + (c > 0 && c < font->numglyph ? c : 0) * font->bytesperglyph;

	for(int i = 0; i < 16; i++) {
		for(int j = 0; j < 8; j++) {
			if(glyph[i] & mask[j]) putPixel(FB_ADDRESS, x+j, y+i-12, color);
		}
	}
} 
*/

