#ifndef TERMINAL_H
#define TERMINAL_H

#include <stdbool.h>

/**
 * Initialize terminal system:
 * - Enables raw mode
 * - Switches to the alternate screen buffer (to preserve user's shell scrollback)
 * - Registers exit hooks for proper restoration
 * Returns true on success, false on failure.
 */
bool terminal_init(void);

/**
 * Shutdown terminal system:
 * - Disables raw mode
 * - Switches back to the main screen buffer
 * - Restores cursor visibility
 */
void terminal_shutdown(void);

/**
 * Set the terminal window title.
 */
void terminal_set_title(const char *title);

#endif // TERMINAL_H
