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
	#ifdef x86_64
	setupGDT();
	#endif

	// ERRORK("testing error\n");

	tPrintStr(" __    __                                                              __         ______    ______   \n", 0xffffff);
	tPrintStr("/  |  /  |                                                            /  |       /      \\  /      \\ \n", 0xffffff);
	tPrintStr("$$ |  $$ | _______   _______    ______   _____  ____    ______    ____$$ |      /$$$$$$  |/$$$$$$  |\n", 0xffffff);
	tPrintStr("$$ |  $$ |/       \\ /       \\  /      \\ /     \\/    \\  /      \\  /    $$ |      $$ |  $$ |$$ \\__$$/ \n", 0xffffff);
	tPrintStr("$$ |  $$ |$$$$$$$  |$$$$$$$  | $$$$$$  |$$$$$$ $$$$  |/$$$$$$  |/$$$$$$$ |      $$ |  $$ |$$      \\ \n", 0xffffff);
	tPrintStr("$$ |  $$ |$$ |  $$ |$$ |  $$ | /    $$ |$$ | $$ | $$ |$$    $$ |$$ |  $$ |      $$ |  $$ | $$$$$$  |\n", 0xffffff);
	tPrintStr("$$ \\__$$ |$$ |  $$ |$$ |  $$ |/$$$$$$$ |$$ | $$ | $$ |$$$$$$$$/ $$ \\__$$ |      $$ \\__$$ |/  \\__$$ |\n", 0xffffff);
	tPrintStr("$$    $$/ $$ |  $$ |$$ |  $$ |$$    $$ |$$ | $$ | $$ |$$       |$$    $$ |      $$    $$/ $$    $$/ \n", 0xffffff);
	tPrintStr(" $$$$$$/  $$/   $$/ $$/   $$/  $$$$$$$/ $$/  $$/  $$/  $$$$$$$/  $$$$$$$/        $$$$$$/   $$$$$$/  \n", 0xffffff);

	// tPrintF(&style, 0xffffff, "Hello World %% %i %s %c %x", 10, "it works?", 'k', 0xfff);
	// printStr("А сейчас проверяю поддержку киррилического алфавита", 10, 28, 0xc4614c);

	Done();
}
