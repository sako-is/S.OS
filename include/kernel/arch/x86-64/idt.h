#pragma once

#include <defines.h>
#ifdef X86_64

typedef struct {
	uint16      offset1;
	uint16      selector;
	uint8       IST;
	uint8       type;
	uint16      offset2;
	uint32      offset3;
	uint32      zero;
} attr(packed) IDT;

typedef struct {
	uint16      size;
	uint64      idt;
} attr(packed) IDTptr;

typedef enum {
	IDTint     = 0xe,
	IDTtrap    = 0xf
} IDTflags;

typedef struct {
	uint64    r15;
	uint64    r14;
	uint64    r13;
	uint64    r12;
	uint64    r11;
	uint64    r10;
	uint64     r9;
	uint64     r8;
	uint64    rbp;
	uint64    rdi;
	uint64    rsi;
	uint64    rdx;
	uint64    rcx;
	uint64    rbx;
	uint64    rax;
	uint64   intn;
	uint64    err;
	uint64    rip;
	uint64     cs;
	uint64 rflags;
	uint64    rsp;
	uint64     ss;
} attr(packed) Registers;

attr(aligned(0x10)) static IDT idt[256];

static IDTptr idtr;

void handleException(Registers* r); // attr(noreturn) Registers* r
void setIDTdescriptor(uint8 vector, void* isr, uint8 flags);
void initIDT(void);
#endif