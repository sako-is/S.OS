#pragma once

#include "printbasic.h"

#include <defines.h>

#include <stdarg.h>

#define ERRORK(message) Print("{#ff0000}ERROR: ");   Print(message);
#define WARNK(message)  Print("{#00ff00}WARNING: "); Print(message);
#define INFOK(message)  Print("{#6A8F54}INFO: ");    Print(message);

typedef struct TerminalContext {
	uint32 margin;
	uint8 tab_size;
	bool is_headless;
	int bg;
	int fg;
    uint32 xsetting;
    uint32 ysetting;
	uint32 x;
	uint32 y;
	PSFfont* font;
} TerminalContext;

typedef struct Stream {
    size_t maxlen;
	size_t len;
	char* buf;
} Stream;

extern Stream term_buf;

extern TerminalContext term_ctx;

void printChar(char c, int color);
void printStr(const char* str, int color); 
void printInt(int intg, int base, int color);

void Print(char* fmt, ...);
void Scroll();