#pragma once

#include <defines.h>

void outb(uint16 port, uint8 val);
uint8 inb(uint16 port);
void IOwait(void);
