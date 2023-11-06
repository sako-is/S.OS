#include <stdarg.h>
#include <kernel/util/memory.h>
#include <kernel/util/util.h>
#include <defines.h>

#define MAXBUF 20000

typedef int (*pfnStreamWriteBuf)(char*);
 
enum ParseMode { NORMAL, ARGUMENT, FORMAT_SPECIFIER };
 
struct Stream {
	size_t buf_len;
	size_t buf_i;
	char *buf;
	pfnStreamWriteBuf pfn_write_all;
};
 
// Returns 0 on success, non-0 on failure.
static int print_stream_buf(struct Stream *stream) {
	// Ensure we are null terminated. Can never be too safe!
	stream->buf[stream->buf_len - 1] = '\0';
 
	// Write the buf.
	if ((size_t)stream->pfn_write_all(stream->buf) != strlen(stream->buf)) return 1;
 
	// Clear the buffer.
	memset(stream->buf, 0, stream->buf_len);
 
	return 0;
}
 
// If not enough space is avilable, call `print_stream_buf`, and forward the
// return code.
static int push_to_buf(struct Stream *stream, char c) {
	stream->buf[stream->buf_i] = c;
	stream->buf_i += 1;
 
	// Check if the buffer is full. If so, flush it.
	if (stream->buf_i == stream->buf_len - 2) {
	int err;
	stream->buf_i = 0;
 
	err = print_stream_buf(stream);
	if (err != 0) {
		// We didn't write the entire buffer, for some reason.
		return err;
	}
	}
 
	return 0;
}
 
// If too little space is in the buffer either at the end of pushing the string
// or at the end, it will call `print_stream_buf`. If that fails, it will
// forward its return code.
//
// As such, returns 0 on success, non-0 on failure.
static int push_all_to_buf(struct Stream *stream, char *str) {
	int err;
 
	for (char c = *str; c != '\0'; c = *++str) {
	err = push_to_buf(stream, c);
 
	if (err != 0) return err;
	}
 
	return 0;
}
 
// To print a signed int this way, first detect if it is negative and then convert it into an
// unsigned type.
static void push_int_to_buf(struct Stream *stream, unsigned int val, unsigned int base) {
	int i = MAXBUF - 2;
	static char buf[MAXBUF] = {0};
 
	for (; val && i; --i, val /= base)
		buf[i] = "0123456789abcdefghijklmnopqrstuvwxyz"[val % base];
 
	push_all_to_buf(stream, &buf[i+1]);
}
 
static int int_display_flag_to_base(char flag) {
	switch (flag) {
	case 'd':
		return 10;
	case 'x':
		return 16;
	case 'o':
		return 8;
	default:
		return 10;
	}
}
 
// The format specifier should follow the below template:
//	{?<arg>.<format code>}
//	note: only include the '?' before the arg if using an arg.
//
// Printing signed number:
//	{?<base>d}
//	note: base can be 'd' for decimal, 'x' for hexidecimal, or 'o' for octal.
//
// Printing unsigned number:
//	{?<base>u}
//	note: base can be 'd' for decimal, 'x' for hexidecimal, or 'o' for octal.
//
// Printing string:
//	{s}
int vfprintf(struct Stream *stream, const char *fmt, va_list args) {
	enum ParseMode parse_mode = NORMAL;
	char arg = '\0';
 
	for (size_t i = 0; i < strlen(fmt); i++) {
		char cur = fmt[i];
 
		switch (cur) {
		case '{': {
			if (parse_mode == NORMAL) {
				// Start looking for arguments next, or another '{' for escape.
				parse_mode = FORMAT_SPECIFIER;
				goto char_loop_end;
			} else {
				// We escaped the '{' character.
				parse_mode = NORMAL;
			}
 
			break;
		}
 
		case '}': {
			if (parse_mode == FORMAT_SPECIFIER) {
				parse_mode = NORMAL;
				goto char_loop_end;
			}
 
			break;
		}
 
		case '?': {
			if (parse_mode == FORMAT_SPECIFIER) {
				parse_mode = ARGUMENT;
				goto char_loop_end;
			}
 
			break;
		}
 
		case 'd': {
			if (parse_mode == FORMAT_SPECIFIER) {
				int val = va_arg(args, int);
 
				if (val < 0)
					push_to_buf(stream, '-');
 
				push_int_to_buf(stream, (unsigned int) val, int_display_flag_to_base(arg));
 
				goto format_spec_end;
			}
 
			break;
		}
 
		case 'u': {
			if (parse_mode == FORMAT_SPECIFIER) {
				unsigned int val = va_arg(args, unsigned int);
				push_int_to_buf(stream, val, int_display_flag_to_base(arg));
 
				goto format_spec_end;
			}
 
			break;
		}
 
		case 's': {
			if (parse_mode == FORMAT_SPECIFIER) {
				char *str = va_arg(args, char *);
				push_all_to_buf(stream, str);
 
				goto format_spec_end;
			}
 
			break;
		}
 
		default: {
			if (parse_mode == ARGUMENT) {
				arg = cur;
				parse_mode = FORMAT_SPECIFIER;
 
				goto char_loop_end;
			}
 
			break;
		}
		}
 
		// Add the current character to the buffer.
		push_to_buf(stream, cur);
 
		goto char_loop_end;
 
format_spec_end:
		arg = '\0';
 
char_loop_end:
		continue;
	}
 
	// At the end, flush the buffer.
	if (print_stream_buf(stream) != 0) {
		// We didn't write the entire buffer, for some reason.
		// TODO: Decide what to do if we ever get here.
	}
 
	// And of course reset the buffer index.
	stream->buf_i = 0;
 

	return 0;
}
