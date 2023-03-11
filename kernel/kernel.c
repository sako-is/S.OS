#include <kernel/kernel.h>
#include <kernel/print/printbasic.h>
#include <kernel/print/print.h>
#include <kernel/arch/common/hal.h>
#include <kernel/util/util.h>

#include <limine/limine.h>

#include <defines.h>

volatile struct limine_framebuffer_request fb_request = {
	.id = LIMINE_FRAMEBUFFER_REQUEST,
	.revision = 0
};

static void Done(void) {
	for(;;) { asm("hlt"); }
}

void kMain(void) {
	fillScreen(darkblue);
	printLogo; printk("\n");

	initHAL();

	Done();
}