#include <kernel/kernel.h>
#include <kernel/print/printbasic.h>
#include <kernel/print/print.h>
#include <kernel/arch/common/hal.h>
#include <kernel/util/util.h>
#include <kernel/malloc.h>
#include <kernel/arch/common/utilcpu.h>

#include <limine/limine.h>

#include <defines.h>

char* strcpy(char* destination, const char* source) {
	if (destination == NULL) return NULL;
	Works();
	char *ptr = destination;
	Works();
 
	while (*source != '\0') {
		*destination = *source;
		destination++;
		source++;
	}
	Works();
 
	*destination = '\0';
	Works();
 
	return ptr;
}

// char* strcpy(char *strDest, const char *strSrc) {
// 	assert(strDest!=NULL && strSrc!=NULL);
// 	char *temp = strDest;
// 	while((*strDest++=*strSrc++) != '\0');
// 	return temp;
// }

volatile struct limine_framebuffer_request fb_request = {
	.id = LIMINE_FRAMEBUFFER_REQUEST,
	.revision = 0
};

static void Done(void) {
	for(;;) { asm("hlt"); }
}

void kMain(void) {
	fillScreen(term_ctx.bg);
	printLogo; printk("\n");

	initHAL();

	char* str = (char*)malloc(15);
	strcpy(str, "Testing malloc\n");
	printk("{s}", str);
	free(str);

	Done();
}
