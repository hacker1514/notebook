#include <stdlib.h>
#include <string.h>
#include "undo.h"
#include "buffer.h"
#include "highlight.h"

static UndoAction *new_action(int type, int cy, int cx, const char *text, int len) {
    UndoAction *action = malloc(sizeof(UndoAction));
    if (action == NULL) return NULL;
    action->type = type;
    action->cy = cy;
    action->cx = cx;
    action->len = len;
    action->next = NULL;
    if (text && len > 0) {
        action->text = malloc(len + 1);
        if (action->text) {
            memcpy(action->text, text, len);
            action->text[len] = '\0';
        }
    } else {
        action->text = NULL;
    }
    return action;
}

static void free_action(UndoAction *action) {
    if (action == NULL) return;
    free(action->text);
    free(action);
}

static void free_stack(UndoAction **stack) {
    UndoAction *curr = *stack;
    while (curr != NULL) {
        UndoAction *next = curr->next;
        free_action(curr);
        curr = next;
    }
    *stack = NULL;
}

void undo_free_all(EditorState *state) {
    free_stack(&state->undo_stack);
    free_stack(&state->redo_stack);
}

static bool is_recording = true;

void undo_record(EditorState *state, int type, int cy, int cx, const char *text, int len) {
    if (!is_recording) return;

    UndoAction *action = new_action(type, cy, cx, text, len);
    if (action == NULL) return;

    action->next = state->undo_stack;
    state->undo_stack = action;

    // Clear redo history when a new action is recorded
    free_stack(&state->redo_stack);
}

static void execute_action(EditorState *state, UndoAction *action, UndoAction **target_stack) {
    is_recording = false;

    if (action->type == ACTION_INSERT) {
        // Inverse of insert is delete
        UndoAction *inv = new_action(ACTION_DELETE, action->cy, action->cx, action->text, action->len);
        if (inv != NULL) {
            inv->next = *target_stack;
            *target_stack = inv;
        }

        for (int i = 0; i < action->len; i++) {
            editor_row_delete_char(&state->row[action->cy], action->cx);
        }
        state->cy = action->cy;
        state->cx = action->cx;
        state->dirty++;
    } 
    else if (action->type == ACTION_DELETE) {
        // Inverse of delete is insert
        UndoAction *inv = new_action(ACTION_INSERT, action->cy, action->cx, action->text, action->len);
        if (inv != NULL) {
            inv->next = *target_stack;
            *target_stack = inv;
        }

        if (action->cy == state->numrows) {
            editor_insert_row(state, state->numrows, "", 0);
        }
        for (int i = 0; i < action->len; i++) {
            editor_row_insert_char(&state->row[action->cy], action->cx + i, action->text[i]);
        }
        state->cy = action->cy;
        state->cx = action->cx + action->len;
        state->dirty++;
    } 
    else if (action->type == ACTION_SPLIT) {
        // Inverse of split is join
        UndoAction *inv = new_action(ACTION_JOIN, action->cy, action->cx, NULL, 0);
        if (inv != NULL) {
            inv->next = *target_stack;
            *target_stack = inv;
        }

        erow *row = &state->row[action->cy + 1];
        erow *prev_row = &state->row[action->cy];
        editor_row_append_string(prev_row, row->chars, row->size);
        editor_delete_row(state, action->cy + 1);

        state->cy = action->cy;
        state->cx = action->cx;
        state->dirty++;
    } 
    else if (action->type == ACTION_JOIN) {
        // Inverse of join is split
        UndoAction *inv = new_action(ACTION_SPLIT, action->cy, action->cx, NULL, 0);
        if (inv != NULL) {
            inv->next = *target_stack;
            *target_stack = inv;
        }

        erow *row = &state->row[action->cy];
        editor_insert_row(state, action->cy + 1, &row->chars[action->cx], row->size - action->cx);
        row = &state->row[action->cy];
        row->size = action->cx;
        row->chars[row->size] = '\0';
        editor_update_row(row);

        state->cy = action->cy + 1;
        state->cx = 0;
        state->dirty++;
    }

    // Trigger syntax updates on affected rows
    if (action->cy < state->numrows) {
        highlight_update_row(state, action->cy);
    }
    if (action->type == ACTION_SPLIT || action->type == ACTION_JOIN) {
        if (action->cy + 1 < state->numrows) {
            highlight_update_row(state, action->cy + 1);
        }
    }

    is_recording = true;
}

void editor_undo(EditorState *state) {
    if (state->undo_stack == NULL) {
        editor_set_status_message(state, "Already at oldest change");
        return;
    }

    UndoAction *action = state->undo_stack;
    state->undo_stack = action->next;

    execute_action(state, action, &state->redo_stack);
    free_action(action);
    editor_set_status_message(state, "Undo completed");
}

void editor_redo(EditorState *state) {
    if (state->redo_stack == NULL) {
        editor_set_status_message(state, "Already at newest change");
        return;
    }

    UndoAction *action = state->redo_stack;
    state->redo_stack = action->next;

    execute_action(state, action, &state->undo_stack);
    free_action(action);
    editor_set_status_message(state, "Redo completed");
}
