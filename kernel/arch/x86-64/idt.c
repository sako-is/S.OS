#include <defines.h>

#ifdef X86_64
#include <kernel/arch/x86-64/idt.h>
#include <kernel/print/print.h>

extern void* isr_stub_table[];

static const char* const Exceptions[] = {
	"Divide by zero error",
	"Debug",
	"Non-maskable Interrupt",
	"Breakpoint",
	"Overflow",
	"Bound Range Exceeded",
	"Invalid Opcode",
	"Device Not Available",
	"Double Fault",
	"Coprocessor Segment Overrun",
	"Invalid TSS",
	"Segment Not Present",
	"Stack-Segment Fault",
	"General Protection Fault",
	"Page Fault",
	"",
	"x87 Floating-Point Exception",
	"Alignment Check",
	"Machine Check",
	"SIMD Floating-Point Exception",
	"Virtualization Exception",
	"Control Protection Exception ",
	"",
	"",
	"",
	"",
	"",
	"",
	"Hypervisor Injection Exception",
	"VMM Communication Exception",
	"Security Exception",
	""
};

void handleException(Registers* r) {
	if(r->intn < 32) Print("EXCEPTION!!! {#ff0000.s}\n", Exceptions[r->intn]);
	else Print("Unhandled Interrupt!!! Interrupt no. {d}\n", r->intn);
	Print("    r15={u}, r14={u}, r13={u}, r12={u}\n", r->r15, r->r14, r->r13, r->r12);
	Print("    r11={u}, r10={u}, r9 ={u}, r8 ={u}\n", r->r11, r->r10, r->r9, r->r8);
	Print("    rbp={u}, rsp={u}, rdi={u}, rsi={u}\n", r->rbp, r->rsp, r->rdi, r->rsi);
	Print("    rdx={u}, rcx={u}, rbx={u}, rax={u}\n", r->rdx, r->rcx, r->rbx, r->rax);
	Print("    error code = {u}\n", r->err);
	asm("hlt");
}

void setIDTdescriptor(uint8 vector, void* isr, uint8 flags) {
	IDT* descriptor = &idt[vector];
 
	descriptor->offset1  = (uint64)isr & 0xFFFF;
	descriptor->selector = 0x08;
	descriptor->IST      = 0;
	descriptor->type     = flags;
	descriptor->offset2  = ((uint64)isr >> 16) & 0xFFFF;
	descriptor->offset3  = ((uint64)isr >> 32) & 0xFFFFFFFF;
	descriptor->zero     = 0;
}

void initIDT(void) {
	idtr.idt = (uint64)&idt[0];
	idtr.size = (uint16)sizeof(IDT) * 256 - 1;
 
	for (uint16 vector = 0; vector < 256; vector++) {
		setIDTdescriptor(vector, isr_stub_table[vector], 0x80 | IDTint);
		// vectors[vector] = true;
	}
 
	asmvol("lidt %0" :: "m"(idtr)); // load the new IDT 
	asmvol("sti"); // set the interrupt flag
}
#endif