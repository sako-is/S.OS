#pragma once

#include <defines.h>

typedef struct {
    uint16      offset1;
    uint16      selector;
    uint8       IST;
    uint8       type;
    uint16      offset2;
    uint32      offset3;
    uint32      zero;
} __attribute__((packed)) IDT;

typedef struct {
    uint16      size;
    IDT*        idt;
} __attribute__((packed)) IDTR;

typedef enum {
    IDT64BitInt     = 0xE,
    IDT64BitTrap    = 0xF
} IDTflags;