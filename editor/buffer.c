#include <stdlib.h>
#include <string.h>
#include "buffer.h"
#include "undo.h"
#include "highlight.h"

int editor_row_cx_to_rx(erow *row, int cx) {
    int rx = 0;
    for (int j = 0; j < cx; j++) {
        if (row->chars[j] == '\t') {
            rx += (NOTEBOOK_TAB_STOP - 1) - (rx % NOTEBOOK_TAB_STOP);
        }
        rx++;
    }
    return rx;
}

void editor_update_row(erow *row) {
    int tabs = 0;
    for (int j = 0; j < row->size; j++) {
        if (row->chars[j] == '\t') tabs++;
    }

    free(row->render);
    row->render = malloc(row->size + tabs * (NOTEBOOK_TAB_STOP - 1) + 1);
    if (row->render == NULL) return;

    int idx = 0;
    for (int j = 0; j < row->size; j++) {
        if (row->chars[j] == '\t') {
            row->render[idx++] = ' ';
            while (idx % NOTEBOOK_TAB_STOP != 0) {
                row->render[idx++] = ' ';
            }
        } else {
            row->render[idx++] = row->chars[j];
        }
    }
    row->render[idx] = '\0';
    row->rsize = idx;
}

void editor_insert_row(EditorState *state, int at, const char *s, size_t len) {
    if (at < 0 || at > state->numrows) return;

    if (state->numrows >= state->row_cap) {
        int new_cap = state->row_cap == 0 ? 64 : state->row_cap * 2;
        erow *new_rows = realloc(state->row, sizeof(erow) * new_cap);
        if (new_rows == NULL) return;
        state->row = new_rows;
        state->row_cap = new_cap;
    }

    memmove(&state->row[at + 1], &state->row[at], sizeof(erow) * (state->numrows - at));

    state->row[at].size = (int)len;
    state->row[at].chars = malloc(len + 1);
    if (state->row[at].chars == NULL) return;
    memcpy(state->row[at].chars, s, len);
    state->row[at].chars[len] = '\0';

    state->row[at].rsize = 0;
    state->row[at].render = NULL;
    state->row[at].hl = NULL;
    state->row[at].hl_open_comment = 0;
    
    editor_update_row(&state->row[at]);
    highlight_update_row(state, at);
    
    state->numrows++;
}

void editor_free_row(erow *row) {
    free(row->render);
    free(row->chars);
    free(row->hl);
}

void editor_row_insert_char(erow *row, int at, int c) {
    if (at < 0 || at > row->size) at = row->size;
    char *new_chars = realloc(row->chars, row->size + 2);
    if (new_chars == NULL) return;
    row->chars = new_chars;

    memmove(&row->chars[at + 1], &row->chars[at], row->size - at + 1);
    row->size++;
    row->chars[at] = c;
    editor_update_row(row);
}

void editor_row_delete_char(erow *row, int at) {
    if (at < 0 || at >= row->size) return;
    memmove(&row->chars[at], &row->chars[at + 1], row->size - at);
    row->size--;
    editor_update_row(row);
}

void editor_row_append_string(erow *row, const char *s, size_t len) {
    char *new_chars = realloc(row->chars, row->size + len + 1);
    if (new_chars == NULL) return;
    row->chars = new_chars;

    memcpy(&row->chars[row->size], s, len);
    row->size += len;
    row->chars[row->size] = '\0';
    editor_update_row(row);
}

void editor_delete_row(EditorState *state, int at) {
    if (at < 0 || at >= state->numrows) return;
    editor_free_row(&state->row[at]);
    memmove(&state->row[at], &state->row[at + 1], sizeof(erow) * (state->numrows - at - 1));
    state->numrows--;
    state->dirty++;
}

void editor_insert_char(EditorState *state, int c) {
    if (state->cy == state->numrows) {
        editor_insert_row(state, state->numrows, "", 0);
    }
    
    // Log insert to history stack
    char s[2] = {c, '\0'};
    undo_record(state, ACTION_INSERT, state->cy, state->cx, s, 1);

    editor_row_insert_char(&state->row[state->cy], state->cx, c);
    state->cx++;
    state->dirty++;
    
    highlight_update_row(state, state->cy);
}

void editor_insert_newline(EditorState *state) {
    // Log newline action to history stack
    undo_record(state, ACTION_SPLIT, state->cy, state->cx, NULL, 0);

    if (state->cx == 0) {
        editor_insert_row(state, state->cy, "", 0);
        state->cy++;
    } else {
        erow *row = &state->row[state->cy];
        editor_insert_row(state, state->cy + 1, &row->chars[state->cx], row->size - state->cx);
        row = &state->row[state->cy];
        row->size = state->cx;
        row->chars[row->size] = '\0';
        editor_update_row(row);
        
        highlight_update_row(state, state->cy);
        state->cy++;
        state->cx = 0;
    }
    state->dirty++;
}

void editor_delete_char(EditorState *state) {
    if (state->cy == state->numrows) return;
    if (state->cx == 0 && state->cy == 0) return;

    erow *row = &state->row[state->cy];
    if (state->cx > 0) {
        // Log delete action to history stack
        char s[2] = {row->chars[state->cx - 1], '\0'};
        undo_record(state, ACTION_DELETE, state->cy, state->cx - 1, s, 1);

        editor_row_delete_char(row, state->cx - 1);
        state->cx--;
        state->dirty++;
        highlight_update_row(state, state->cy);
    } else {
        // Log line join to history stack
        erow *prev_row = &state->row[state->cy - 1];
        undo_record(state, ACTION_JOIN, state->cy - 1, prev_row->size, NULL, 0);

        state->cx = prev_row->size;
        editor_row_append_string(prev_row, row->chars, row->size);
        editor_delete_row(state, state->cy);
        state->cy--;
        highlight_update_row(state, state->cy);
    }
}
