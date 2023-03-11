#include <defines.h>

#ifdef X86_64
#include <kernel/arch/x86-64/pic.h>

#include <kernel/arch/common/utilcpu.h>

void PICsendEOI(uint8 irq) {
	if(irq >= 8) outb(PIC2_COMMAND, PIC_EOI);
	outb(PIC1_COMMAND, PIC_EOI);
}

void initPIC(int offset1, int offset2) {
	unsigned char a1, a2;
 
	a1 = inb(PIC1_DATA);                        // save masks
	a2 = inb(PIC2_DATA);
 
	outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);  // starts the initialization sequence (in cascade mode)
	IOwait();
	outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
	IOwait();
	outb(PIC1_DATA, offset1);                 // ICW2: Master PIC vector offset
	IOwait();
	outb(PIC2_DATA, offset2);                 // ICW2: Slave PIC vector offset
	IOwait();
	outb(PIC1_DATA, 4);                       // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
	IOwait();
	outb(PIC2_DATA, 2);                       // ICW3: tell Slave PIC its cascade identity (0000 0010)
	IOwait();
 
	outb(PIC1_DATA, ICW4_8086);
	IOwait();
	outb(PIC2_DATA, ICW4_8086);
	IOwait();
 
	outb(PIC1_DATA, a1);   // restore saved masks.
	outb(PIC2_DATA, a2);
}

#endif