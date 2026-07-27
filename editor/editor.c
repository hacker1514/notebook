#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include "editor.h"
#include "platform.h"
#include "keymap.h"
#include "terminal.h"
#include "screen.h"
#include "cursor.h"
#include "buffer.h"
#include "file.h"
#include "undo.h"
#include "search.h"
#include "theme.h"
#include "plugins.h"
#include "prompt.h"
#include "clipboard.h"
#include "command.h"
#include "help.h"

static int close_confirm = 0;

// Swaps the active editing parameters back to the index registry
static void buffer_save_active(EditorState *state) {
    if (state->num_buffers == 0 || state->active_buffer < 0) return;
    EditorBuffer *buf = &state->buffers[state->active_buffer];
    buf->filename = state->filename;
    buf->numrows = state->numrows;
    buf->row = state->row;
    buf->row_cap = state->row_cap;
    buf->cx = state->cx;
    buf->cy = state->cy;
    buf->rx = state->rx;
    buf->rowoff = state->rowoff;
    buf->coloff = state->coloff;
    buf->dirty = state->dirty;
    buf->syntax = state->syntax;
    buf->undo_stack = state->undo_stack;
    buf->redo_stack = state->redo_stack;
    buf->line_ending = state->line_ending;

    // Selection fields
    buf->sel_sx = state->sel_sx;
    buf->sel_sy = state->sel_sy;
    buf->sel_ex = state->sel_ex;
    buf->sel_ey = state->sel_ey;
    buf->selection_active = state->selection_active;
}

// Swaps the editing parameters of buffer 'idx' into the active workspace
static void buffer_load_active(EditorState *state, int idx) {
    if (idx < 0 || idx >= state->num_buffers) return;
    state->active_buffer = idx;
    EditorBuffer *buf = &state->buffers[idx];
    state->filename = buf->filename;
    state->numrows = buf->numrows;
    state->row = buf->row;
    state->row_cap = buf->row_cap;
    state->cx = buf->cx;
    state->cy = buf->cy;
    state->rx = buf->rx;
    state->rowoff = buf->rowoff;
    state->coloff = buf->coloff;
    state->dirty = buf->dirty;
    state->syntax = buf->syntax;
    state->undo_stack = buf->undo_stack;
    state->redo_stack = buf->redo_stack;
    state->line_ending = buf->line_ending;

    // Selection fields
    state->sel_sx = buf->sel_sx;
    state->sel_sy = buf->sel_sy;
    state->sel_ex = buf->sel_ex;
    state->sel_ey = buf->sel_ey;
    state->selection_active = buf->selection_active;

    // Update the terminal window title
    char title[256];
    if (state->filename) {
        snprintf(title, sizeof(title), "Notebook - %s", state->filename);
    } else {
        snprintf(title, sizeof(title), "Notebook - [New File]");
    }
    terminal_set_title(title);
}

void buffer_new(EditorState *state) {
    buffer_save_active(state);

    EditorBuffer *new_bufs = realloc(state->buffers, sizeof(EditorBuffer) * (state->num_buffers + 1));
    if (new_bufs == NULL) return;
    state->buffers = new_bufs;

    EditorBuffer *buf = &state->buffers[state->num_buffers];
    buf->filename = NULL;
    buf->numrows = 0;
    buf->row = NULL;
    buf->row_cap = 0;
    buf->cx = 0;
    buf->cy = 0;
    buf->rx = 0;
    buf->rowoff = 0;
    buf->coloff = 0;
    buf->dirty = 0;
    buf->syntax = NULL;
    buf->undo_stack = NULL;
    buf->redo_stack = NULL;
    buf->sel_sx = 0;
    buf->sel_sy = 0;
    buf->sel_ex = 0;
    buf->sel_ey = 0;
    buf->selection_active = 0;
#ifdef _WIN32
    buf->line_ending = LE_CRLF;
#else
    buf->line_ending = LE_LF;
#endif

    state->num_buffers++;
    buffer_load_active(state, state->num_buffers - 1);
    editor_set_status_message(state, "Created new empty buffer");
}

bool buffer_open(EditorState *state, const char *filename) {
    // Check if filename target is already open in another buffer
    for (int i = 0; i < state->num_buffers; i++) {
        if (state->buffers[i].filename && strcmp(state->buffers[i].filename, filename) == 0) {
            buffer_save_active(state);
            buffer_load_active(state, i);
            editor_set_status_message(state, "Switched to buffer %d", i + 1);
            return true;
        }
    }

    // Allocate new buffer workspace and load
    buffer_new(state);
    if (!file_open(state, filename)) {
        editor_set_status_message(state, "New file registered: %s", filename);
    } else {
        editor_set_status_message(state, "Loaded: %s", filename);
        plugins_trigger(state, EVENT_ON_LOAD, NULL);
    }
    return true;
}

void buffer_close(EditorState *state) {
    if (state->num_buffers == 0) return;

    if (state->dirty && close_confirm < 1) {
        editor_set_status_message(state, "Warning! Unsaved changes. Press Ctrl+W again to force close.");
        close_confirm++;
        return;
    }
    close_confirm = 0;

    // Free the active buffer's contents
    if (state->row) {
        for (int i = 0; i < state->numrows; i++) {
            editor_free_row(&state->row[i]);
        }
        free(state->row);
    }
    free(state->filename);
    undo_free_all(state);

    int idx = state->active_buffer;
    
    // Shift subsequent elements left
    memmove(&state->buffers[idx], &state->buffers[idx + 1], sizeof(EditorBuffer) * (state->num_buffers - idx - 1));
    state->num_buffers--;

    if (state->num_buffers == 0) {
        // Last buffer was closed: reset buffers array and spin up a new empty buffer
        state->buffers = NULL;
        state->active_buffer = -1;
        buffer_new(state);
        editor_set_status_message(state, "Closed file. Created blank buffer.");
    } else {
        EditorBuffer *new_bufs = realloc(state->buffers, sizeof(EditorBuffer) * state->num_buffers);
        if (state->num_buffers > 0 && new_bufs != NULL) {
            state->buffers = new_bufs;
        }
        
        int next_active = idx;
        if (next_active >= state->num_buffers) {
            next_active = state->num_buffers - 1;
        }
        buffer_load_active(state, next_active);
    }
}

void buffer_cycle(EditorState *state, int dir) {
    if (state->num_buffers <= 1) return;
    buffer_save_active(state);
    int target = (state->active_buffer + dir + state->num_buffers) % state->num_buffers;
    buffer_load_active(state, target);
    editor_set_status_message(state, "Switched to buffer %d/%d", target + 1, state->num_buffers);
}

bool editor_init(EditorState *state) {
    state->screenrows = 0;
    state->screencols = 0;
    state->quit = false;

    // Initialize multi-buffer variables
    state->buffers = NULL;
    state->active_buffer = -1;
    state->num_buffers = 0;

    // Clear active pointer duplicates
    state->cx = 0;
    state->cy = 0;
    state->rx = 0;
    state->rowoff = 0;
    state->coloff = 0;
    state->numrows = 0;
    state->row = NULL;
    state->row_cap = 0;
    state->dirty = 0;
    state->filename = NULL;
    state->syntax = NULL;
    state->undo_stack = NULL;
    state->redo_stack = NULL;
    
    // Clear selection
    state->sel_sx = 0;
    state->sel_sy = 0;
    state->sel_ex = 0;
    state->sel_ey = 0;
    state->selection_active = 0;

    state->statusmsg[0] = '\0';
    state->statusmsg_time = 0;
    state->mode = MODE_INSERT;
    state->line_ending = LE_LF;

    // Select default One Dark color theme
    theme_select(state, "onedark");

    // Fetch initial terminal dimensions
    if (!platform_terminal_size(&state->screenrows, &state->screencols)) {
        state->screenrows = 24;
        state->screencols = 80;
    }
    state->screenrows -= 2; // Leave room for status bar and notification prompt

    return true;
}

void editor_cleanup(EditorState *state) {
    buffer_save_active(state);

    if (state->buffers) {
        for (int i = 0; i < state->num_buffers; i++) {
            EditorBuffer *buf = &state->buffers[i];
            if (buf->row) {
                for (int r = 0; r < buf->numrows; r++) {
                    editor_free_row(&buf->row[r]);
                }
                free(buf->row);
            }
            free(buf->filename);

            // Free undo/redo chains for this buffer
            UndoAction *curr = buf->undo_stack;
            while (curr != NULL) {
                UndoAction *next = curr->next;
                free(curr->text);
                free(curr);
                curr = next;
            }
            curr = buf->redo_stack;
            while (curr != NULL) {
                UndoAction *next = curr->next;
                free(curr->text);
                free(curr);
                curr = next;
            }
        }
        free(state->buffers);
        state->buffers = NULL;
    }
    state->num_buffers = 0;
    state->active_buffer = -1;

    // Free global clipboard memory
    clipboard_free_global();
}

void editor_set_status_message(EditorState *state, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(state->statusmsg, sizeof(state->statusmsg), fmt, ap);
    va_end(ap);
    state->statusmsg_time = time(NULL);
}

static void editor_process_keypress(EditorState *state) {
    static int quit_confirm = 0;
    int c = platform_get_key();

    // Global action keys (always active regardless of mode)
    if (c == CTRL_KEY('q')) {
        if (state->dirty && quit_confirm < 1) {
            editor_set_status_message(state, "Warning! File has unsaved changes. Press Ctrl+Q again to force quit.");
            quit_confirm++;
            return;
        }
        state->quit = true;
        return;
    }

    if (c == CTRL_KEY('s')) {
        editor_save(state);
        // Fire hook after saving
        plugins_trigger(state, EVENT_ON_SAVE, NULL);
        return;
    }

    if (c == CTRL_KEY('f')) {
        editor_find(state);
        return;
    }

    if (c == CTRL_KEY('n')) {
        buffer_new(state);
        return;
    }

    if (c == CTRL_KEY('o')) {
        char *fn = editor_prompt(state, "Open file: %s (ESC to cancel)", NULL);
        if (fn != NULL) {
            buffer_open(state, fn);
            free(fn);
        }
        return;
    }

    if (c == CTRL_KEY('w')) {
        buffer_close(state);
        return;
    }

    if (c == KEY_F1) {
        editor_open_help_manual(state);
        return;
    }

    if (c == KEY_F5) {
        theme_cycle(state);
        return;
    }

    if (c == KEY_RESIZE) {
        platform_terminal_size(&state->screenrows, &state->screencols);
        state->screenrows -= 2;
        return;
    }

    // Global copy/paste/select shortcuts
    if (c == CTRL_KEY('a')) {
        editor_select_all(state);
        return;
    }
    if (c == CTRL_KEY('c')) {
        editor_copy(state);
        return;
    }
    if (c == CTRL_KEY('v')) {
        state->selection_active = 0; // Paste clears selection
        editor_paste(state);
        return;
    }

    int cursor_moved = 0;

    // Modal key routing
    if (state->mode == MODE_NORMAL) {
        switch (c) {
            case 'i':
                state->mode = MODE_INSERT;
                state->selection_active = 0; // Entering insert clears selection
                editor_set_status_message(state, "-- INSERT MODE --");
                break;

            case ':':
                state->mode = MODE_INSERT; // Command prompt temporarily acts as text prompt
                editor_enter_command_mode(state);
                state->mode = MODE_NORMAL;
                break;

            // Vim Visual Mode trigger
            case 'v':
                if (state->selection_active) {
                    state->selection_active = 0;
                    editor_set_status_message(state, "-- NORMAL MODE --");
                } else {
                    state->selection_active = 1;
                    state->sel_sy = state->cy;
                    state->sel_sx = state->cx;
                    state->sel_ey = state->cy;
                    state->sel_ex = state->cx;
                    editor_set_status_message(state, "-- VISUAL SELECT --");
                }
                break;

            // Vim style navigation shortcuts
            case 'h':
                cursor_move(state, KEY_ARROW_LEFT);
                cursor_moved = 1;
                break;
            case 'l':
                cursor_move(state, KEY_ARROW_RIGHT);
                cursor_moved = 1;
                break;
            case 'k':
                cursor_move(state, KEY_ARROW_UP);
                cursor_moved = 1;
                break;
            case 'j':
                cursor_move(state, KEY_ARROW_DOWN);
                cursor_moved = 1;
                break;

            // Vim style editing shortcuts
            case 'u':
                state->selection_active = 0;
                editor_undo(state);
                break;
            case 'r':
                state->selection_active = 0;
                editor_redo(state);
                break;

            // Buffer cycling in Normal Mode (Tab = Next, Shift+Tab = Prev)
            case KEY_TAB:
                state->selection_active = 0;
                buffer_cycle(state, 1);
                break;
            case KEY_SHIFT_TAB:
                state->selection_active = 0;
                buffer_cycle(state, -1);
                break;

            // Fallback standard navigation keys
            case KEY_ARROW_UP:
            case KEY_ARROW_DOWN:
            case KEY_ARROW_LEFT:
            case KEY_ARROW_RIGHT:
            case KEY_PAGE_UP:
            case KEY_PAGE_DOWN:
            case KEY_HOME:
            case KEY_END:
                cursor_move(state, c);
                cursor_moved = 1;
                break;

            default:
                // Ignore other keys in Normal Mode
                break;
        }
    } else {
        // INSERT MODE
        switch (c) {
            case KEY_ESCAPE:
                state->mode = MODE_NORMAL;
                state->selection_active = 0;
                editor_set_status_message(state, "-- NORMAL MODE --");
                break;

            case KEY_ENTER:
            case KEY_CARRIAGE_RETURN:
                state->selection_active = 0;
                editor_insert_newline(state);
                break;

            case KEY_BACKSPACE:
            case CTRL_KEY('h'):
                state->selection_active = 0;
                editor_delete_char(state);
                break;

            case KEY_DELETE:
                state->selection_active = 0;
                cursor_move(state, KEY_ARROW_RIGHT);
                editor_delete_char(state);
                break;

            case KEY_ARROW_UP:
            case KEY_ARROW_DOWN:
            case KEY_ARROW_LEFT:
            case KEY_ARROW_RIGHT:
            case KEY_PAGE_UP:
            case KEY_PAGE_DOWN:
            case KEY_HOME:
            case KEY_END:
                cursor_move(state, c);
                cursor_moved = 1;
                break;

            case KEY_TAB:
                state->selection_active = 0;
                editor_insert_char(state, '\t');
                break;

            case CTRL_KEY('z'):
                state->selection_active = 0;
                editor_undo(state);
                break;

            case CTRL_KEY('y'):
                state->selection_active = 0;
                editor_redo(state);
                break;

            default:
                if (c >= 32 && c < 127) {
                    state->selection_active = 0;
                    editor_insert_char(state, c);
                    // Fire key trigger event for plugins (auto brackets)
                    plugins_trigger(state, EVENT_ON_KEY, &c);
                }
                break;
        }
    }

    // Dynamic selection extension during cursor updates
    if (cursor_moved && state->selection_active) {
        state->sel_ex = state->cx;
        state->sel_ey = state->cy;
    }

    // Reset confirmations if respective keys are not clicked
    if (c != CTRL_KEY('q')) {
        quit_confirm = 0;
    }
    if (c != CTRL_KEY('w')) {
        close_confirm = 0;
    }
}

void editor_run(EditorState *state) {
    if (!terminal_init()) {
        fprintf(stderr, "Failed to initialize terminal\n");
        return;
    }

    // Initialize and trigger on-load hooks
    plugins_init();
    plugins_trigger(state, EVENT_ON_LOAD, NULL);

    // Initial status message
    editor_set_status_message(state, "HELP: F1 = Manual | Escape = Normal | :help = Command Mode | Ctrl+A/C/V = Clipboard");

    while (!state->quit) {
        screen_refresh(state);
        editor_process_keypress(state);
    }

    platform_clear_screen();
    terminal_shutdown();
}
