#pragma once

#include <limine/limine.h>

#include <defines.h>

extern volatile struct limine_framebuffer_request fb_request;

#define FB_ADDRESS fb_request.response->framebuffers[0]->address

#define printLogo	printk("{#fe370c}  ______       ______    ______  \n"); \
					printk("{#fe370c} /      \\     /      \\  /      \\ \n"); \
					printk("{#fe370c}/$$$$$$  |   /$$$$$$  |/$$$$$$  |\n");    \
					printk("{#fe370c}$$ \\__$$/    $$ |  $$ |$$ \\__$$/ \n");  \
					printk("{#fe370c}$$      \\    $$ |  $$ |$$      \\ \n");  \
					printk("{#fe370c} $$$$$$  |   $$ |  $$ | $$$$$$  |\n");    \
					printk("{#fe370c}/  \\__$$ |__ $$ \\__$$ |/  \\__$$ |\n"); \
					printk("{#fe370c}$$    $$//  |$$    $$/ $$    $$/ \n");    \
					printk("{#fe370c} $$$$$$/ $$/  $$$$$$/   $$$$$$/  \n");
