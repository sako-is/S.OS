#pragma once

#include <defines.h>

typedef struct Terminal {
    uint32 margin;
    uint32 x;
    uint32 y;
} Terminal;

Terminal terminalInit();

void TTYPrintStr(Terminal* tty, const char* str, int color);