#include <defines.h>

#ifdef X86_64
#include <kernel/arch/common/serialports.h>
#include <kernel/arch/common/utilcpu.h>

bool isTransmitEmpty() {
	return inb(PORT + 5) & 0x20;
}

void writeSerial(char a) {
	while(isTransmitEmpty() == false);

	outb(PORT, a);
}

#endif
