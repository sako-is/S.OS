#include <kernel/print/print.h>
#include <kernel/kernel.h>
#include <kernel/print/printbasic.h>
#include <kernel/util/util.h>
#include <kernel/arch/common/serialports.h>

#include <defines.h>

#include <stdarg.h>

#define NORMAL_STATE 1
#define FORMAT_STATE 2
#define COLOR_STATE 3

TerminalContext term_ctx = {
	.margin			= 5,
	.is_headless	= false,
	.bg				= darkblue,
	.fg				= white,
	.tab_size		= 4,
	.x				= 5,
	.y				= 5,
	.font			= (PSFfont*)&_binary_assets_Tamsyn8x16r_psf_start
};

Stream term_buf = {
	.buf_len	= 0,
	.buf		= NULL
};

void printChar(char c, int color) {
	if(c == '\n') { term_ctx.x = term_ctx.margin; term_ctx.y += term_ctx.font->height + 1; return; }
	if(c == '\t') { term_ctx.x += term_ctx.tab_size * (term_ctx.font->width + 1); return; }
	tPrintChar(c, term_ctx.x, term_ctx.y, color);
	term_ctx.x += term_ctx.font->width + 1;
}

void printStr(const char* str, int color) {
	int max_chars_per_line = (fb_request.response->framebuffers[0]->width - 2*term_ctx.margin - term_ctx.x) / (term_ctx.font->width + 1);

	for (int i = 0, j = 0; i < (int)strlen(str); i++, j++) {
		if (str[i] == '\n') { j = -1; term_ctx.x = term_ctx.margin; term_ctx.y += term_ctx.font->height + 1; continue; }
		else if (j >= max_chars_per_line) { j = -1; i--; term_ctx.x = term_ctx.margin; term_ctx.y += term_ctx.font->height + 1; continue; }

		tPrintChar(str[i], term_ctx.x, term_ctx.y, color);
		term_ctx.x += term_ctx.font->width+1;
	}
}

void printInt(int intg, int base, int color) {
	int max_chars_per_line = (fb_request.response->framebuffers[0]->width - 2*term_ctx.margin - term_ctx.x) / (term_ctx.font->width + 1);
	char buffer[33];
	char* str = itoa(intg, buffer, base);

	for (int i = 0, j = 0; i < (int)strlen((const char*)str); i++, j++) {
		if (str[i] == '\n') { j = -1; term_ctx.x = term_ctx.margin; term_ctx.y += term_ctx.font->height + 1; continue; }
		else if (j >= max_chars_per_line) { j = -1; i--; term_ctx.x = term_ctx.margin; term_ctx.y += term_ctx.font->height + 1; continue; }

		tPrintChar(str[i], term_ctx.x, term_ctx.y, color);
		term_ctx.x += term_ctx.font->width+1;
	}
}

void printUint(uint32 intg, int base, int color) {
	int max_chars_per_line = (fb_request.response->framebuffers[0]->width - 2*term_ctx.margin - term_ctx.x) / (term_ctx.font->width + 1);
	char buffer[33];
	char* str = uitoa(intg, buffer, base);

	for (int i = 0, j = 0; i < (int)strlen((const char*)str); i++, j++) {
		if (str[i] == '\n') { j = -1; term_ctx.x = term_ctx.margin; term_ctx.y += term_ctx.font->height + 1; continue; }
		else if (j >= max_chars_per_line) { j = -1; i--; term_ctx.x = term_ctx.margin; term_ctx.y += term_ctx.font->height + 1; continue; }

		tPrintChar(str[i], term_ctx.x, term_ctx.y, color);
		term_ctx.x += term_ctx.font->width+1;
	}
}

uint64 xtou64(const char* str) {
	uint64 res = 0;
	char c;

	while ((c = *str++)) {
		char v = (c & 0xF) + (c >> 6) | ((c >> 3) & 0x8);
		res = (res << 4) | (uint64)v;
	}

	return res;
}

void printk(char* fmt, ...) {
	va_list ap;
	int color = term_ctx.fg;

	void (*printint)(int, int, int);
	void (*printuint)(uint32, int, int);
	void (*printchar)(char, int);

	if(term_ctx.is_headless) {
		printint = NULL;
		printuint = NULL;
		printchar = NULL;
	} else {
		printint = printInt;
		printuint = printUint;
		printchar = printChar;
	}

	char* p, *sval;
	int ival;
	uint32 uval;
	char cval;

	int state = 1;
	char sc[6];

	va_start(ap, fmt);
	for(p = fmt; *p; p++) {
		writeSerial(*p);
		switch(state) {
			case NORMAL_STATE:
				switch(*p) {
					case '{': state = FORMAT_STATE; break;
					default: printchar(*p, color); break;
				}
				break;
			case FORMAT_STATE:
				switch(*p) {
					case 'd': ival = va_arg(ap, int); printint(ival, 10, color); break;
					case 'o': ival = va_arg(ap, int); printint(ival, 8, color); break;
					case 'x': ival = va_arg(ap, int); printint(ival, 16, color); break;
					case 'u': uval = va_arg(ap, uint32); printuint(uval, 10, color); break;
					case 'y': uval = va_arg(ap, uint32); printuint(uval, 8, color); break;
					case 'X': uval = va_arg(ap, uint32); printuint(uval, 16, color); break;
					case 's':
						for (sval = va_arg(ap, char *); *sval; sval++) printchar(*sval, color);
						break;
					case 'c': cval = (char)va_arg(ap, int); printchar(cval, color); break;
					case '#': state = COLOR_STATE; break;
					case '}': state = NORMAL_STATE; break;
					default: printchar(*p, color); break;
				}
				break;
			case COLOR_STATE:
				switch(*p) {
					case '}': state = NORMAL_STATE; break;
					case '.': state = FORMAT_STATE; break;
					default:
						for(int i = 0; i < 6; i++) sc[i] = *(p+i);
						color = (int)xtou64((const char*)sc);
						p+=5;
						break;
				}
				break;
		}
	}
	va_end(ap);
}

