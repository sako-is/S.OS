#pragma once

#include <limine/limine.h>

extern volatile struct limine_hhdm_request hhdm_request;

#define VMM_HIGHER_HALF (hhdm_request.response->offset)
