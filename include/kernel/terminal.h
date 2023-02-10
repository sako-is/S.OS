#pragma once

#include <defines.h>

static const size_t VGA_WIDTH  = 80;
static const size_t VGA_HEIGHT = 250;

typedef enum VgaColor {
    Black        = 0,
    Blue         = 1,
    Green        = 2,
    Cyan         = 3,
    Red          = 4,
    Magenta      = 5,
    Brown        = 6,
    LightGrey    = 7,
    DarkGrey     = 8,
    LightBlue    = 9,
    LightGreen   = 10,
    LightCyan    = 11,
    LightRed     = 12,
    LightMagenta = 13,
    LightBrown   = 14,
    White        = 15
} VgaColor;

void terminalInit();
void terminalSetColor(uint8 color);

void terminalPutEntryAt(char c, uint8 color, size_t x, size_t y);
void terminalPutChar(char c);

void terminalWrite(const char* str, size_t len);
void terminalPrint(const char* str);