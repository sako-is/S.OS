#include <kernel/kernel.h>
#include <kernel/terminal.h>
// #include <limine/limine.h>

#include <defines.h>

// static volatile struct limine_terminal_request terminal_request = {
//     .id = LIMINE_TERMINAL_REQUEST,
//     .revision = 0
// };
// static void kMain(void);

// struct limine_entry_point_request entry_point_request = {
//     .id = LIMINE_ENTRY_POINT_REQUEST,
//     .revision = 0, .response = NULL,

//     .entry = kMain
// };

static void Done(void) {
    for(;;) { }
}

void kMain(void) {
	terminalInit();
    terminalPrint("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");

    // if (terminal_request.response == NULL
    //  || terminal_request.response->terminal_count < 1) {
    //     Done();
    // }
 
    // // We should now be able to call the Limine terminal to print out
    // // a simple "Hello World" to screen.
    // struct limine_terminal *terminal = terminal_request.response->terminals[0];
    // terminal_request.response->write(terminal, "Hello World", 11);

    Done();
}