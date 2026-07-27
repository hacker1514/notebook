#include <stdlib.h>
#include <string.h>
#include "clipboard.h"
#include "buffer.h"

static char *clipboard = NULL;
static int clipboard_len = 0;

static void get_normalized_selection(EditorState *state, int *sy, int *sx, int *ey, int *ex) {
    if (state->sel_sy < state->sel_ey) {
        *sy = state->sel_sy; *sx = state->sel_sx;
        *ey = state->sel_ey; *ex = state->sel_ex;
    } else if (state->sel_sy > state->sel_ey) {
        *sy = state->sel_ey; *sx = state->sel_ex;
        *ey = state->sel_sy; *ex = state->sel_sx;
    } else {
        *sy = state->sel_sy;
        *ey = state->sel_ey;
        if (state->sel_sx <= state->sel_ex) {
            *sx = state->sel_sx; *ex = state->sel_ex;
        } else {
            *sx = state->sel_ex; *ex = state->sel_sx;
        }
    }
}

void editor_select_all(EditorState *state) {
    if (state->numrows == 0) return;
    state->sel_sy = 0;
    state->sel_sx = 0;
    state->sel_ey = state->numrows - 1;
    state->sel_ex = state->row[state->numrows - 1].size;
    state->selection_active = 1;
    editor_set_status_message(state, "Selected all text");
}

void editor_copy(EditorState *state) {
    if (!state->selection_active) {
        editor_set_status_message(state, "No selection active to copy");
        return;
    }

    int sy, sx, ey, ex;
    get_normalized_selection(state, &sy, &sx, &ey, &ex);

    // Calculate total buffer length required
    int copy_len = 0;
    for (int y = sy; y <= ey; y++) {
        if (y == sy && y == ey) {
            copy_len += (ex - sx);
        } else if (y == sy) {
            copy_len += (state->row[y].size - sx) + 1; // +1 for '\n'
        } else if (y == ey) {
            copy_len += ex;
        } else {
            copy_len += state->row[y].size + 1; // +1 for '\n'
        }
    }

    free(clipboard);
    clipboard = malloc(copy_len + 1);
    if (clipboard == NULL) {
        editor_set_status_message(state, "Copy failed: Out of memory");
        return;
    }

    char *p = clipboard;
    for (int y = sy; y <= ey; y++) {
        erow *row = &state->row[y];
        if (y == sy && y == ey) {
            memcpy(p, &row->chars[sx], ex - sx);
            p += (ex - sx);
        } else if (y == sy) {
            memcpy(p, &row->chars[sx], row->size - sx);
            p += (row->size - sx);
            *p++ = '\n';
        } else if (y == ey) {
            memcpy(p, row->chars, ex);
            p += ex;
        } else {
            memcpy(p, row->chars, row->size);
            p += row->size;
            *p++ = '\n';
        }
    }
    *p = '\0';
    clipboard_len = copy_len;

    // Reset selection state
    state->selection_active = 0;
    editor_set_status_message(state, "Copied %d characters", clipboard_len);
}

void editor_paste(EditorState *state) {
    if (clipboard == NULL || clipboard_len == 0) {
        editor_set_status_message(state, "Clipboard is empty");
        return;
    }

    // Insert clipboard characters sequentially. This ensures proper undo stacking.
    for (int i = 0; i < clipboard_len; i++) {
        char c = clipboard[i];
        if (c == '\n') {
            editor_insert_newline(state);
        } else {
            editor_insert_char(state, c);
        }
    }
    editor_set_status_message(state, "Pasted %d characters", clipboard_len);
}

void clipboard_free_global(void) {
    free(clipboard);
    clipboard = NULL;
    clipboard_len = 0;
}
