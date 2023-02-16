#include <kernel/kernel.h>
#include <kernel/print.h>
#include <kernel/terminal.h>

#include <limine/limine.h>

#include <defines.h>

volatile struct limine_framebuffer_request fb_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0
};

static void Done(void) {
    for(;;) { __asm__("hlt"); }
}

void kMain(void) {
    Terminal term = terminalInit();
    TTYPrintStr(&term, "Here's some text to test the terminal :) ", 0x3035d4);
    TTYPrintStr(&term, "Here's another one aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", 0xabd430);
    // TTYPrintStr(&term, "hi", 0xffffff);
    // printStr("А сейчас проверяю поддержку киррилического алфавита", 10, 28, 0xc4614c);

    Done();
}