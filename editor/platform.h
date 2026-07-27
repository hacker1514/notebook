#ifndef PLATFORM_H
#define PLATFORM_H

#include <stdbool.h>

/**
 * Initialize platform-specific systems (e.g., UTF-8 console output,
 * virtual terminal sequence processing on Windows).
 * Returns true on success, false on failure.
 */
bool platform_init(void);

/**
 * Enable raw terminal input mode.
 * Returns true on success, false on failure.
 */
bool platform_enable_raw_mode(void);

/**
 * Restore terminal to original cooked mode.
 */
void platform_disable_raw_mode(void);

/**
 * Read a single keystroke.
 * Handles multibyte sequences (like arrow keys) and maps them to custom keycodes.
 * Returns the keycode, or -1 on error/EOF.
 */
int platform_get_key(void);

/**
 * Move the terminal cursor to 0-indexed (row, col) position.
 */
void platform_move_cursor(int row, int col);

/**
 * Hide the terminal cursor.
 */
void platform_hide_cursor(void);

/**
 * Show the terminal cursor.
 */
void platform_show_cursor(void);

/**
 * Clear the entire terminal screen.
 */
void platform_clear_screen(void);

/**
 * Retrieve the current terminal dimensions (rows and columns).
 * Returns true on success, false on failure.
 */
bool platform_terminal_size(int *rows, int *cols);

/**
 * Pause execution for a given number of milliseconds.
 */
void platform_sleep_ms(unsigned int ms);

#endif // PLATFORM_H
