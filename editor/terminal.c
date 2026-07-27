#include <stdio.h>
#include <stdlib.h>
#include "terminal.h"
#include "platform.h"

static bool terminal_initialized = false;

static void terminal_atexit_handler(void) {
    terminal_shutdown();
}

bool terminal_init(void) {
    if (terminal_initialized) return true;

    // Initialize platform system (e.g. VT mode, UTF-8 output)
    if (!platform_init()) return false;

    // Enable raw mode input
    if (!platform_enable_raw_mode()) return false;

    // Switch to alternate screen buffer and clear/reset cursor position
    printf("\033[?1049h\033[H");
    fflush(stdout);

    // Register clean cleanup handler
    atexit(terminal_atexit_handler);
    terminal_initialized = true;

    return true;
}

void terminal_shutdown(void) {
    if (!terminal_initialized) return;

    // Switch back to primary screen buffer and restore cursor
    printf("\033[?1049l\033[?25h");
    fflush(stdout);

    platform_disable_raw_mode();
    terminal_initialized = false;
}

void terminal_set_title(const char *title) {
    // OSC command to set terminal title: ESC ] 0 ; <title> BEL
    printf("\033]0;%s\007", title);
    fflush(stdout);
}
