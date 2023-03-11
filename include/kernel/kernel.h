#pragma once

#include <limine/limine.h>

#include <defines.h>

extern volatile struct limine_framebuffer_request fb_request;

#define FB_ADDRESS fb_request.response->framebuffers[0]->address

#define printLogo 	printk("  ______       ______    ______  \n"); \
					printk(" /      \\     /      \\  /      \\ \n"); \
					printk("/$$$$$$  |   /$$$$$$  |/$$$$$$  |\n");    \
					printk("$$ \\__$$/    $$ |  $$ |$$ \\__$$/ \n");  \
					printk("$$      \\    $$ |  $$ |$$      \\ \n");  \
					printk(" $$$$$$  |   $$ |  $$ | $$$$$$  |\n");    \
					printk("/  \\__$$ |__ $$ \\__$$ |/  \\__$$ |\n"); \
					printk("$$    $$//  |$$    $$/ $$    $$/ \n");    \
					printk(" $$$$$$/ $$/  $$$$$$/   $$$$$$/  \n");