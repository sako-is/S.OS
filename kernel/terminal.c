#include <kernel/terminal.h>
#include <kernel/kernel.h>
#include <kernel/print.h>
#include <kernel/util.h>

Terminal terminalInit() {
    Terminal term = { 10, 10, 10 };

    return term;
}

void TTYPrintStr(Terminal* tty, const char* str, int color) {
    PSFfont* font = (PSFfont*)&_binary_assets_Tamsyn8x16r_psf_start;
    int max_chars_per_line = (fb_request.response->framebuffers[0]->width - 2*tty->margin - tty->x) / (font->width + 1);

    for (int i = 0, j = 0; i < (int)strlen(str); i++, j++) {
        if (str[i] == '\n') { j = -1; i--; tty->x = tty->margin; tty->y += font->height + 1; continue; }
        else if (j >= max_chars_per_line) { j = -1; i--; tty->x = tty->margin; tty->y += font->height + 1; continue; }

        printChar(str[i], tty->x, tty->y, color);
        tty->x += font->width+1;
    }

    tty->x = (uint32)strlen(str) * (font->width + 1) % (fb_request.response->framebuffers[0]->width - 2*tty->margin) + font->width + 1;
    // tty->y += (uint32)((uint32)strlen(str)*9 % fb_request.response->framebuffers[0]->pitch);
}