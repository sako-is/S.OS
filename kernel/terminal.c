#include <kernel/terminal.h>

#include <kernel/util.h>
#include <defines.h>

size_t terminal_row;
size_t terminal_column;
uint8 terminal_color;
uint16* terminal_buffer;

static inline uint8 vgaEntryColor(VgaColor fg, VgaColor bg) {
    return fg | bg << 4;
}

static inline uint16 vgaEntry(unsigned char uc, uint8 color) {
    return (uint16)uc | (uint16)color << 8;
}

void terminalInit() {
    terminal_row = 0;
    terminal_column = 0;
    terminal_color = vgaEntryColor(LightGrey, Black);
    terminal_buffer = (uint16*)0xb8000;
    
    for(size_t y=0; y < VGA_HEIGHT; y++) {
        for(size_t x=0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            terminal_buffer[index] = vgaEntry(' ', terminal_color);
        }
    }
}

void terminalSetColor(uint8 color) {
    terminal_color = color;
}

void terminalPutEntryAt(char c, uint8 color, size_t x, size_t y) {
    const size_t index = y * VGA_WIDTH + x;
    terminal_buffer[index] = vgaEntry(c, color);
}

void terminalPutChar(char c) {
    terminalPutEntryAt(c, terminal_color, terminal_row, terminal_column);
    if(++terminal_column == VGA_WIDTH) {
        terminal_column = 0;
        if(++terminal_row == VGA_HEIGHT) terminal_row = 0;
    }
}

void terminalWrite(const char* str, size_t len) {
    for(size_t i=0; i < len; i++) terminalPutChar(str[i]);
}

void terminalPrint(const char* str) {
    terminalWrite(str, strlen(str));
}