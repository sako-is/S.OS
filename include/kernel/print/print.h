#pragma once

#include "printbasic.h"

#include <defines.h>

#define ERRORK(message) printk("{fe370c}ERROR: ");   printk(message, 0xfe370c);
#define WARNK(message)  printk("{fe370c}WARNING: "); printk(message, 0xf3fe0c);
#define INFOK(message)  printk("{fe370c}INFO: ");    printk(message, 0xf9f7f4);

typedef struct Style {
	uint32 margin;
	uint32 x;
	uint32 y;
	PSFfont* font;
} Style;

extern Style style;

void printChar(char c, int color);
void printStr(const char* str, int color); 
void printInt(int intg, int base, int color);

void printk(char* fmt, ...);