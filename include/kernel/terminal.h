#pragma once

#include "print.h"

#include <defines.h>

#define ERRORK(message) tPrintStr(message, 0xfe370c);
#define WARNK(message) tPrintStr(message, 0xf3fe0c);

typedef struct Style {
    uint32 margin;
    uint32 x;
    uint32 y;
    PSFfont* font;
} Style;

extern Style style;

void tPrintStr(const char* str, int color); 
void tPrintInt(int intg, int base, int color);
void tPrintF(int color, const char* fmt, ...);