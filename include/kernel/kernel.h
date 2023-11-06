#pragma once

#include <limine/limine.h>

#include <defines.h>

extern volatile struct limine_framebuffer_request fb_request;

#define FB_ADDRESS fb_request.response->framebuffers[0]->address

#define printLogo()	Print("{#fe0c37}  ______       ______    ______  \n"); \
					Print("{#fe0c37} /      \\     /      \\  /      \\ \n"); \
					Print("{#fe0c37}/$$$$$$  |   /$$$$$$  |/$$$$$$  |\n");    \
					Print("{#fe0c37}$$ \\__$$/    $$ |  $$ |$$ \\__$$/ \n");  \
					Print("{#fe0c37}$$      \\    $$ |  $$ |$$      \\ \n");  \
					Print("{#fe0c37} $$$$$$  |   $$ |  $$ | $$$$$$  |\n");    \
					Print("{#fe0c37}/  \\__$$ |__ $$ \\__$$ |/  \\__$$ |\n"); \
					Print("{#fe0c37}$$    $$//  |$$    $$/ $$    $$/ \n");    \
					Print("{#fe0c37} $$$$$$/ $$/  $$$$$$/   $$$$$$/  \n");
