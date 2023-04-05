#include <defines.h>
#ifdef X86_64
#include <kernel/arch/common/hal.h>

#include <kernel/arch/x86-64/gdt.h>
#include <kernel/arch/x86-64/idt.h>
#include <kernel/arch/x86-64/pic.h>
#include <kernel/arch/x86-64/apic.h>

#include <kernel/memory/pmm.h>
#include <kernel/malloc.h>

#include <kernel/print/print.h>
#include <kernel/print/printbasic.h>

extern void disablePIC();

void initHAL() {
	asm("cli");					// Disable Interrupts
	
	setupGDT();
	INFOK("GDT initialized\n");

	initIDT();                  // Interrupts enabled again
	INFOK("IDT initialized\n");
	
	initPMM();
	INFOK("PMM initialized\n");

	initMalloc();
	INFOK("Malloc initialized\n");
	/*
	if(!checkAPIC()) {
		for(int i = 0; i < 8; i++) initPIC(0x08 + i, 0x70 + i);
		INFOK("PIC Initialized\n");
	} else {
		#ifdef __PHYSICAL_MEMORY_EXTENSION__
		INFOK("Physical Memory Extension exists\n");
		#endif
		initAPIC();
		INFOK("APIC Initialized\n");
	}
*/
}
#endif
