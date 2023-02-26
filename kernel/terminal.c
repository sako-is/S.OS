#include <kernel/terminal.h>
#include <kernel/kernel.h>
#include <kernel/print.h>
#include <kernel/util.h>

#include <stdarg.h>

#define TPRINTF_NORMAL      0
#define TPRINTF_LENGTH      1
#define TPRINTF_SHORT_STATE 2
#define TPRINTF_LONG_STATE  3
#define TPRINTF_SPEC        4

#define TPRINTF_DEFAULT_LENGTH 0
#define TPRINTF_SHORT_SHORT    1
#define TPRINTF_SHORT          2
#define TPRINTF_LONG           3
#define TPRINTF_LONG_LONG      4

Style style = {
	.margin = 0, 
	.x = 0, 
	.y = 0, 
	.font = (PSFfont*)&_binary_assets_Tamsyn8x16r_psf_start
};

void tPrintChar(char c, int color) {
	PSFfont* font = style.font;

	if(c == '\n') { style.x = style.margin; style.y += font->height + 1; return; }
	printChar(c, style.x, style.y, color);
	style.x += font->width+1;

}

void tPrintStr(const char* str, int color) {
	PSFfont* font = style.font;
	int max_chars_per_line = (fb_request.response->framebuffers[0]->width - 2*style.margin - style.x) / (font->width + 1);

	for (int i = 0, j = 0; i < (int)strlen(str); i++, j++) {
		if (str[i] == '\n') { j = -1; style.x = style.margin; style.y += font->height + 1; continue; }
		else if (j >= max_chars_per_line) { j = -1; i--; style.x = style.margin; style.y += font->height + 1; continue; }

		printChar(str[i], style.x, style.y, color);
		style.x += font->width+1;
	}

	// style->y += (uint32)((uint32)strlen(str)*9 % fb_request.response->framebuffers[0]->pitch);
}

void tPrintInt(int intg, int base, int color) {
	PSFfont* font = style.font;
	int max_chars_per_line = (fb_request.response->framebuffers[0]->width - 2*style.margin - style.x) / (font->width + 1);
	char buffer[33];
	char* str = itoa(intg, buffer, base);

	for (int i = 0, j = 0; i < (int)strlen((const char*)str); i++, j++) {
		if (str[i] == '\n') { j = -1; style.x = style.margin; style.y += font->height + 1; continue; }
		else if (j >= max_chars_per_line) { j = -1; i--; style.x = style.margin; style.y += font->height + 1; continue; }

		printChar(str[i], style.x, style.y, color);
		style.x += font->width+1;
	}
}

/*

void tPrintF(Style* style, int color, const char* fmt, ...) {
	va_list ap;
	int state = TPRINTF_NORMAL;
	int length;
	int radix = 10;

	va_start(ap,)
	while(*fmt) {
		switch(state) {
		case TPRINTF_NORMAL:
			switch (*fmt) {
			case '%':
				state = TPRINTF_LENGTH;
				break;
			default: 
				tPrintChar(tty, *fmt, color);
				break;
			}
			break;
		case TPRINTF_LENGTH:
			switch (*fmt) {
			case 'h':
				length = TPRINTF_SHORT;
				state = TPRINTF_SHORT_STATE;
				break;
			case 'l':
				length = TPRINTF_LONG;
				state = TPRINTF_LONG_STATE;
				break;
			default:
				goto TPRINTF_SPEC_;
			}
			break;
		case TPRINTF_SHORT_STATE:
			if(*fmt == 'h') {
				length = TPRINTF_SHORT_SHORT;
				state = TPRINTF_SPEC;
			} else goto TPRINTF_SPEC_;
			break;
		case TPRINTF_LONG_STATE:
			if(*fmt == 'h') {
				length = TPRINTF_LONG_LONG;
				state = TPRINTF_SPEC;
			} else goto TPRINTF_SPEC_;
			break;
		case TPRINTF_SPEC:
		TPRINTF_SPEC_:
			switch (*fmt)
			{
			case 'c':
				tPrintChar(tty, (char)*argp, color);
				argp++;
				break;
			case 's':
				tPrintStr(tty, *(const char**)argp, color);
				argp++;
				break;
			case '%':
				tPrintChar(tty, '%', color);
				argp++;
				break;
			case 'd':
			case 'i': 
				radix = 10;
				tPrintInt(tty, *(int*)argp, radix, color);
				argp++;
				break;
			case 'u': 
				radix = 10;
				tPrintInt(tty, *(int*)argp, radix, color);
				argp++;
				break;
			case 'X':
			case 'x':
			case 'p':
				radix = 16;
				tPrintInt(tty, *(int*)argp, radix, color);
				argp++;
				break;
			case 'o':
				radix = 8;
				tPrintInt(tty, *(int*)argp, radix, color);
				argp++;
				break;
			default:
				break;
			}
			state = TPRINTF_NORMAL;
			length = TPRINTF_DEFAULT_LENGTH;
			radix = 10;
		}
		fmt++;
	}
}
*/