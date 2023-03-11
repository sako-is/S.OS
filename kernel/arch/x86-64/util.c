#include <defines.h>
#ifdef X86_64

void outb(uint16 port, uint8 val) {
	asmvol("outb %0, %1" : : "a"(val), "Nd"(port));
}

uint8 inb(uint16 port) {
	uint8 ret;
	asmvol("inb %1, %0" : "=a"(ret) : "Nd"(port));
	return ret;
}

void IOwait(void) {
	outb(0x80, 0);
}
#endif