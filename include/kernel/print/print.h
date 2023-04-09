#pragma once

#include "printbasic.h"

#include <defines.h>

#include <stdarg.h>

#define ERRORK(message) printk("{#ff0000}ERROR: ");   printk(message);
#define WARNK(message)  printk("{#0000ff}WARNING: "); printk(message);
#define INFOK(message)  printk("{#00ff00}INFO: ");    printk(message);

typedef struct TerminalContext {
	uint32 margin;
	uint8 tab_size;
	bool is_headless;
	int bg;
	int fg;
	uint32 x;
	uint32 y;
	PSFfont* font;
} TerminalContext;

// typedef struct Stream {
// 	size_t buf_len;
// 	va_list ap;
// 	char* buf;
// } Stream;

// extern Stream term_buf;

extern TerminalContext term_ctx;

void printChar(char c, int color);
void printStr(const char* str, int color); 
void printInt(int intg, int base, int color);

void printk(char* fmt, ...);
