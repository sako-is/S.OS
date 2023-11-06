#include <kernel/print/print.h>
#include <kernel/kernel.h>
#include <kernel/malloc.h>
#include <kernel/print/printbasic.h>
#include <kernel/util/util.h>
#include <kernel/util/memory.h>
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
	.xsetting       = 5,
	.ysetting       = 5,
	.x				= 5,
	.y				= 5,
	.font			= (PSFfont*)&_binary_assets_Tamsyn8x16r_psf_start
};

Stream term_buf = {
	.maxlen         = 50000 * sizeof(char),
	.len			= 0,
	.buf			= NULL
};

void printChar(char c, int color) {
	// term_buf.len++;
	// if(term_buf.len >= term_buf.maxlen) {
	// 	term_buf.maxlen += term_buf.len;
	// 	term_buf.buf = realloc(term_buf.buf, term_buf.maxlen);
	// }
	// term_buf.buf[term_buf.len] = c;
	
	if(c == '\n') { 
		term_ctx.x = term_ctx.margin; 
		term_ctx.y += term_ctx.font->height + 1; 
		return; 
	}
	if(c == '\t') { 
		term_ctx.x += term_ctx.tab_size * (term_ctx.font->width + 1); 
		return; 
	}
	tPrintChar(c, term_ctx.x, term_ctx.y, color);
	term_ctx.x += term_ctx.font->width + 1;
	if (term_ctx.x >= fb_request.response->framebuffers[0]->width - 2*term_ctx.margin) {
		term_ctx.x = term_ctx.margin;
		term_ctx.y += term_ctx.font->height +1;
	}
}

void printStr(const char* str, int color) {
	int max_chars_per_line = (fb_request.response->framebuffers[0]->width - 2*term_ctx.margin) / (term_ctx.font->width + 1);

	/*
	* i = character ID
	* j = x position
	*/
	for (int i = 0, j = 0; i < (int)strlen(str); i++, j++) {
		if (str[i] == '\n') { 
			j = -1; 
			term_ctx.x = term_ctx.margin; 
			term_ctx.y += term_ctx.font->height + 1; 
			continue; }
		else if (j >= max_chars_per_line) { 
			j = -1; 
			i--; 
			term_ctx.x = term_ctx.margin; 
			term_ctx.y += term_ctx.font->height + 1; 
			continue; 
		}

		// if(term_ctx.y <= fb_request.response->framebuffers[0]->height - term_ctx.ysetting)
		// 	Scroll();

		tPrintChar(str[i], term_ctx.x, term_ctx.y, color);
		term_ctx.x += term_ctx.font->width+1;
	}
}

void printInt(int intg, int base, int color) {
	int max_chars_per_line = (fb_request.response->framebuffers[0]->width - 2*term_ctx.margin - term_ctx.x) / (term_ctx.font->width + 1);
	char buffer[33];
	char* str = itoa(intg, buffer, base);

	for (int i = 0, j = 0; i < (int)strlen((const char*)str); i++, j++) {
		if (str[i] == '\n') { 
			j = -1; term_ctx.x = term_ctx.margin; 
			term_ctx.y += term_ctx.font->height + 1; 
			continue; }
		else if (j >= max_chars_per_line) { 
			j = -1; i--; 
			term_ctx.x = term_ctx.margin; 
			term_ctx.y += term_ctx.font->height + 1; 
			continue; 
		}

		tPrintChar(str[i], term_ctx.x, term_ctx.y, color);
		term_ctx.x += term_ctx.font->width+1;
	}
}

void printUint(uint32 intg, int base, int color) {
	int max_chars_per_line = (fb_request.response->framebuffers[0]->width - 2*term_ctx.margin - term_ctx.x) / (term_ctx.font->width + 1);
	char buffer[33];
	char* str = uitoa(intg, buffer, base);

	for (int i = 0, j = 0; i < (int)strlen((const char*)str); i++, j++) {
		if (str[i] == '\n') { 
			j = -1; term_ctx.x = term_ctx.margin; 
			term_ctx.y += term_ctx.font->height + 1; 
			continue; 
		}
		else if (j >= max_chars_per_line) { 
			j = -1; i--; 
			term_ctx.x = term_ctx.margin; 
			term_ctx.y += term_ctx.font->height + 1; 
			continue; 
		}

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

/*
void Print(char* fmt, ...) {
	va_list ap;
	int color = term_ctx.fg;

	void (*printInt)(int, int, int);
	void (*printUint)(uint32, int, int);
	void (*printChar)(char, int);
	void (*cls)(int);
	void (*serial)(char);

	serial = NULL;

	if(term_ctx.is_headless) {
		printInt = NULL;
		printUint = NULL;
		printChar = NULL;
		cls = NULL;
	} else {
		printInt = printInt;
		printUint = printUint;
		printChar = printChar;
		cls = fillScreen;
	}

	char* p, *sval;
	int ival;
	uint32 uval;
	char cval;

	int state = 1;
	char sc[6];
	char buffer[33];
	char* intstr;

	va_start(ap, fmt);
	for(p = fmt; *p; p++) {
		switch(state) {
			case NORMAL_STATE:
				switch(*p) {
					case '{':
						if(*(p+1) == '#') { serial(*p); term_buf.buf[term_buf.len + 1] = *p; }
						state = FORMAT_STATE; 
						break;
					default:
						serial(*p);
						term_buf.buf[term_buf.len + 1] = *p;
						printchar(*p, color); break;
				}
				break;
			case FORMAT_STATE:
				switch(*p) {
					case 'd':
						ival = va_arg(ap, int);
						intstr = itoa(ival, buffer, 10);
						for(size_t i = 0; i < strlen(intstr); i++, term_buf.len++) {
							serial(intstr[i]);
							term_buf.buf[term_buf.len+1] = intstr[i];
						}
						memset(buffer, 0, strlen(buffer));
						printInt(ival, 10, color); 
						break;
					case 'o': 
						ival = va_arg(ap, int); 
						intstr = itoa(ival, buffer, 8);
						for(size_t i = 0; i < strlen(intstr); i++, term_buf.len++) {
							serial(intstr[i]);
							term_buf.buf[term_buf.len+1] = intstr[i];
						}
						memset(buffer, 0, strlen(buffer));
						
						printInt(ival, 8, color); 
						break;
					case 'x': 
						ival = va_arg(ap, int); 
						intstr = itoa(ival, buffer, 16);
						for(size_t i = 0; i < strlen(intstr); i++, term_buf.len++) {
							serial(intstr[i]);
							term_buf.buf[term_buf.len+1] = intstr[i];
						}
						memset(buffer, 0, strlen(buffer));
						
						printInt(ival, 16, color); 
						break;
					case 'u': 
						uval = va_arg(ap, uint32); 
						intstr = uitoa(ival, buffer, 10);
						for(size_t i = 0; i < strlen(intstr); i++, term_buf.len++) {
							serial(intstr[i]);
							term_buf.buf[term_buf.len+1] = intstr[i];
						}
						memset(buffer, 0, strlen(buffer));
						printUint(uval, 10, color); 
						break;
					case 'y': 
						uval = va_arg(ap, uint32); 
						intstr = uitoa(ival, buffer, 10);
						for(size_t i = 0; i < strlen(intstr); i++, term_buf.len++) {
							serial(intstr[i]);
							term_buf.buf[term_buf.len+1] = intstr[i];
						}
						memset(buffer, 0, strlen(buffer));
						printUint(uval, 8, color); 
						break;
					case 'X': 
						uval = va_arg(ap, uint32); 
						intstr = uitoa(ival, buffer, 10);
						for(size_t i = 0; i < strlen(intstr); i++, term_buf.len++) {
							serial(intstr[i]);
							term_buf.buf[term_buf.len+1] = intstr[i];
						}
						memset(buffer, 0, strlen(buffer));
						printUint(uval, 16, color); 
						break;
					case 's':
						for (sval = va_arg(ap, char *); *sval; sval++, term_buf.len++) { 
							serial(*sval);
							term_buf.buf[term_buf.len+1] = *sval;
							printchar(*sval, color);
						}
						break;
					case 'c': 
						cval = (char)va_arg(ap, int);
						serial(cval);
						term_buf.buf[term_buf.len+1] = cval;
						term_buf.len++;
						printchar(cval, color); 
						break;
					case '#':
						serial('{');
						serial(*p);
						term_buf.buf[term_buf.len + 1] = '{';
						term_buf.buf[term_buf.len + 2] = *p;
						term_buf.len += 2;
						state = COLOR_STATE; break;
					case '}': state = NORMAL_STATE; break;
					default:
						term_buf.buf[term_buf.len + 1] = *p;
						term_buf.len++;
						printchar(*p, color); 
						break;
				}
				break;
			case COLOR_STATE:
				switch(*p) {
					case '}':
						serial(*p);
						term_buf.buf[term_buf.len + 1] = *p;
						term_buf.len++;
						state = NORMAL_STATE; 
						break;
					case '.':
						serial('}');
						term_buf.buf[term_buf.len + 1] = '}';
						term_buf.len++;
						state = FORMAT_STATE; 
						break;
					default:
						for(int i = 0; i < 6; i++, term_buf.len++) { 
							sc[i] = *(p+i);
							serial(*(p+i));
							term_buf.buf[term_buf.len + 1] = *(p+i);
						}
						color = (int)xtou64((const char*)sc);
						p+=5;
						break;
				}
				break;
		}
		if(term_ctx.y >= fb_request.response->framebuffers[0]->height) cls(term_ctx.bg);
	}
	va_end(ap);
}
*/

void Print(char* fmt, ...) {
	va_list ap;
	int color = term_ctx.fg;

	char* p, *sval;
	int ival;
	uint32 uval;
	char cval;

	int state = 1;
	char sc[6];

	va_start(ap, fmt);
	for(p = fmt; *p; p++) {
		// if(term_ctx.y >= fb_request.response->framebuffers[0]->height - term_ctx.ysetting)
		// 	Scroll();

		writeSerial(*p);
		switch(state) {
			case NORMAL_STATE:
				switch(*p) {
					case '{': state = FORMAT_STATE; break;
					default: printChar(*p, color); break;
				}
				break;
			case FORMAT_STATE:
				switch(*p) {
					case 'd': 
						ival = va_arg(ap, int); 
						printInt(ival, 10, color); 
						break;
					case 'o': ival = va_arg(ap, int); printInt(ival, 8, color); break;
					case 'x': ival = va_arg(ap, int); printInt(ival, 16, color); break;
					case 'u': uval = va_arg(ap, uint32); printUint(uval, 10, color); break;
					case 'y': uval = va_arg(ap, uint32); printUint(uval, 8, color); break;
					case 'X': uval = va_arg(ap, uint32); printUint(uval, 16, color); break;
					case 's':
						for (sval = va_arg(ap, char *); *sval; sval++) printChar(*sval, color);
						break;
					case 'c': cval = (char)va_arg(ap, int); printChar(cval, color); break;
					case '#': state = COLOR_STATE; break;
					case '}': state = NORMAL_STATE; break;
					default: printChar(*p, color); break;
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

void Scroll() {

}