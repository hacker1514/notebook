#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif
#ifndef ENABLE_WINDOW_INPUT
#define ENABLE_WINDOW_INPUT 0x0008
#endif

#include <conio.h>
#include <stdio.h>
#include <stdbool.h>
#include "platform.h"
#include "keymap.h"

static HANDLE hStdIn = INVALID_HANDLE_VALUE;
static DWORD origInMode = 0;
static bool rawModeEnabled = false;

static int last_rows = 0;
static int last_cols = 0;

bool platform_init(void) {
    hStdIn = GetStdHandle(STD_INPUT_HANDLE);
    if (hStdIn == INVALID_HANDLE_VALUE) return false;

    // Set console output code page to UTF-8
    SetConsoleOutputCP(CP_UTF8);

    // Enable Virtual Terminal Processing for stdout (native ANSI escape codes)
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hStdOut != INVALID_HANDLE_VALUE) {
        DWORD mode = 0;
        if (GetConsoleMode(hStdOut, &mode)) {
            mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(hStdOut, mode);
        }
    }

    // Initialize dimensions
    platform_terminal_size(&last_rows, &last_cols);

    return true;
}

bool platform_enable_raw_mode(void) {
    if (hStdIn == INVALID_HANDLE_VALUE) {
        hStdIn = GetStdHandle(STD_INPUT_HANDLE);
    }
    if (!GetConsoleMode(hStdIn, &origInMode)) return false;

    DWORD rawMode = origInMode;
    // Disable line input, echo input, processed input.
    // Keep ENABLE_WINDOW_INPUT to receive resize events.
    rawMode &= ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT | ENABLE_PROCESSED_INPUT);
    rawMode |= ENABLE_WINDOW_INPUT;

    if (!SetConsoleMode(hStdIn, rawMode)) return false;
    rawModeEnabled = true;
    return true;
}

void platform_disable_raw_mode(void) {
    if (rawModeEnabled && hStdIn != INVALID_HANDLE_VALUE) {
        SetConsoleMode(hStdIn, origInMode);
        rawModeEnabled = false;
    }
}

int platform_get_key(void) {
    if (!rawModeEnabled) {
        return _getch();
    }

    INPUT_RECORD irInBuf[128];
    DWORD cNumRead;

    while (true) {
        if (!ReadConsoleInputW(hStdIn, irInBuf, 128, &cNumRead)) {
            return -1;
        }

        for (DWORD i = 0; i < cNumRead; i++) {
            if (irInBuf[i].EventType == KEY_EVENT) {
                KEY_EVENT_RECORD ker = irInBuf[i].Event.KeyEvent;
                if (ker.bKeyDown) {
                    DWORD ctrlState = ker.dwControlKeyState;
                    bool ctrlPressed = (ctrlState & LEFT_CTRL_PRESSED) || (ctrlState & RIGHT_CTRL_PRESSED);

                    // Check control mappings first
                    if (ctrlPressed && ker.wVirtualKeyCode >= 'A' && ker.wVirtualKeyCode <= 'Z') {
                        return CTRL_KEY(ker.wVirtualKeyCode);
                    }

                    // Special virtual keys
                    switch (ker.wVirtualKeyCode) {
                        case VK_LEFT:
                            return ctrlPressed ? KEY_CTRL_LEFT : KEY_ARROW_LEFT;
                        case VK_RIGHT:
                            return ctrlPressed ? KEY_CTRL_RIGHT : KEY_ARROW_RIGHT;
                        case VK_UP:
                            return ctrlPressed ? KEY_CTRL_UP : KEY_ARROW_UP;
                        case VK_DOWN:
                            return ctrlPressed ? KEY_CTRL_DOWN : KEY_ARROW_DOWN;
                        case VK_PRIOR:
                            return KEY_PAGE_UP;
                        case VK_NEXT:
                            return KEY_PAGE_DOWN;
                        case VK_HOME:
                            return ctrlPressed ? KEY_CTRL_HOME : KEY_HOME;
                        case VK_END:
                            return ctrlPressed ? KEY_CTRL_END : KEY_END;
                        case VK_DELETE:
                            return KEY_DELETE;
                        case VK_INSERT:
                            return KEY_INSERT;
                        case VK_ESCAPE:
                            return KEY_ESCAPE;
                        case VK_TAB:
                            return (ctrlState & SHIFT_PRESSED) ? KEY_SHIFT_TAB : KEY_TAB;
                        case VK_BACK:
                            return KEY_BACKSPACE;
                        case VK_RETURN:
                            // Map return to carriage return or enter
                            return KEY_CARRIAGE_RETURN;
                        case VK_F1:  return KEY_F1;
                        case VK_F2:  return KEY_F2;
                        case VK_F3:  return KEY_F3;
                        case VK_F4:  return KEY_F4;
                        case VK_F5:  return KEY_F5;
                        case VK_F6:  return KEY_F6;
                        case VK_F7:  return KEY_F7;
                        case VK_F8:  return KEY_F8;
                        case VK_F9:  return KEY_F9;
                        case VK_F10: return KEY_F10;
                        case VK_F11: return KEY_F11;
                        case VK_F12: return KEY_F12;
                    }

                    WCHAR unicodeChar = ker.uChar.UnicodeChar;
                    if (unicodeChar != 0) {
                        return unicodeChar;
                    }
                }
            } else if (irInBuf[i].EventType == WINDOW_BUFFER_SIZE_EVENT) {
                int r, c;
                if (platform_terminal_size(&r, &c)) {
                    if (r != last_rows || c != last_cols) {
                        last_rows = r;
                        last_cols = c;
                        return KEY_RESIZE;
                    }
                }
            }
        }
    }
    return -1;
}

void platform_move_cursor(int row, int col) {
    printf("\033[%d;%dH", row + 1, col + 1);
    fflush(stdout);
}

void platform_hide_cursor(void) {
    printf("\033[?25l");
    fflush(stdout);
}

void platform_show_cursor(void) {
    printf("\033[?25h");
    fflush(stdout);
}

void platform_clear_screen(void) {
    printf("\033[2J\033[H");
    fflush(stdout);
}

bool platform_terminal_size(int *rows, int *cols) {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) return false;
    if (GetConsoleScreenBufferInfo(hOut, &csbi)) {
        *cols = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        *rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
        return true;
    }
    return false;
}

void platform_sleep_ms(unsigned int ms) {
    Sleep(ms);
}

#else // POSIX (Linux, macOS, BSD, etc.)
#define _DEFAULT_SOURCE
#define _BSD_SOURCE
#define _GNU_SOURCE

#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <signal.h>
#include "platform.h"
#include "keymap.h"

static struct termios orig_termios;
static bool raw_mode_enabled = false;
static volatile sig_atomic_t got_resize = 0;

static void sigwinch_handler(int sig) {
    (void)sig;
    got_resize = 1;
}

bool platform_init(void) {
    struct sigaction sa;
    sa.sa_handler = sigwinch_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGWINCH, &sa, NULL);
    return true;
}

bool platform_enable_raw_mode(void) {
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1) return false;

    struct termios raw = orig_termios;
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1; // 100ms timeout

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) return false;
    raw_mode_enabled = true;
    return true;
}

void platform_disable_raw_mode(void) {
    if (raw_mode_enabled) {
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
        raw_mode_enabled = false;
    }
}

int platform_get_key(void) {
    char c;
    int nread;

    while (true) {
        if (got_resize) {
            got_resize = 0;
            return KEY_RESIZE;
        }

        nread = read(STDIN_FILENO, &c, 1);
        if (nread == -1) {
            if (errno == EINTR) {
                if (got_resize) {
                    got_resize = 0;
                    return KEY_RESIZE;
                }
                continue;
            }
            return -1;
        }

        if (nread == 0) {
            // Wait for input or signal using select
            fd_set fds;
            FD_ZERO(&fds);
            FD_SET(STDIN_FILENO, &fds);
            int ret = select(STDIN_FILENO + 1, &fds, NULL, NULL, NULL);
            if (ret == -1 && errno == EINTR && got_resize) {
                got_resize = 0;
                return KEY_RESIZE;
            }
            continue;
        }

        if (c == 27) { // Escape character
            char seq[5];
            fd_set fds;
            struct timeval tv = {0, 50000}; // 50ms timeout
            FD_ZERO(&fds);
            FD_SET(STDIN_FILENO, &fds);

            if (select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv) <= 0) {
                return KEY_ESCAPE;
            }

            if (read(STDIN_FILENO, &seq[0], 1) == 0) return KEY_ESCAPE;
            if (read(STDIN_FILENO, &seq[1], 1) == 0) return KEY_ESCAPE;

            if (seq[0] == '[') {
                if (seq[1] >= '0' && seq[1] <= '9') {
                    if (read(STDIN_FILENO, &seq[2], 1) == 0) return KEY_ESCAPE;
                    if (seq[2] == '~') {
                        switch (seq[1]) {
                            case '1': return KEY_HOME;
                            case '3': return KEY_DELETE;
                            case '4': return KEY_END;
                            case '5': return KEY_PAGE_UP;
                            case '6': return KEY_PAGE_DOWN;
                            case '7': return KEY_HOME;
                            case '8': return KEY_END;
                        }
                    } else if (seq[2] == ';') {
                        if (read(STDIN_FILENO, &seq[3], 1) == 0) return KEY_ESCAPE;
                        if (read(STDIN_FILENO, &seq[4], 1) == 0) return KEY_ESCAPE;
                        if (seq[1] == '1' && seq[3] == '5') {
                            switch (seq[4]) {
                                case 'A': return KEY_CTRL_UP;
                                case 'B': return KEY_CTRL_DOWN;
                                case 'C': return KEY_CTRL_RIGHT;
                                case 'D': return KEY_CTRL_LEFT;
                            }
                        }
                    } else {
                        char final_char;
                        if (read(STDIN_FILENO, &final_char, 1) == 0) return KEY_ESCAPE;
                        if (final_char == '~') {
                            int code = (seq[1] - '0') * 10 + (seq[2] - '0');
                            if (code == 15) return KEY_F5;
                        }
                    }
                } else {
                    switch (seq[1]) {
                        case 'A': return KEY_ARROW_UP;
                        case 'B': return KEY_ARROW_DOWN;
                        case 'C': return KEY_ARROW_RIGHT;
                        case 'D': return KEY_ARROW_LEFT;
                        case 'H': return KEY_HOME;
                        case 'F': return KEY_END;
                        case 'Z': return KEY_SHIFT_TAB; // Shift+Tab escape code \x1b[Z
                    }
                }
            } else if (seq[0] == 'O') {
                switch (seq[1]) {
                    case 'H': return KEY_HOME;
                    case 'F': return KEY_END;
                    case 'P': return KEY_F1;  // F1 key
                    case 'Q': return KEY_F2;  // F2 key
                    case 'R': return KEY_F3;  // F3 key
                    case 'S': return KEY_F4;  // F4 key
                }
            }
            return KEY_ESCAPE;
        }

        return c;
    }
}

void platform_move_cursor(int row, int col) {
    printf("\033[%d;%dH", row + 1, col + 1);
    fflush(stdout);
}

void platform_hide_cursor(void) {
    printf("\033[?25l");
    fflush(stdout);
}

void platform_show_cursor(void) {
    printf("\033[?25h");
    fflush(stdout);
}

void platform_clear_screen(void) {
    printf("\033[2J\033[H");
    fflush(stdout);
}

bool platform_terminal_size(int *rows, int *cols) {
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
        return false;
    } else {
        *cols = ws.ws_col;
        *rows = ws.ws_row;
        return true;
    }
}

void platform_sleep_ms(unsigned int ms) {
    usleep(ms * 1000);
}

#endif
