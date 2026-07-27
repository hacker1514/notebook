#include <stdlib.h>
#include <string.h>
#include "search.h"
#include "prompt.h"
#include "highlight.h"
#include "keymap.h"

static void editor_find_callback(EditorState *state, const char *query, int key) {
    static int last_match = -1;
    static int direction = 1;

    // Reset parameters on termination
    if (key == KEY_ENTER || key == KEY_CARRIAGE_RETURN || key == KEY_ESCAPE) {
        last_match = -1;
        direction = 1;
        return;
    }

    // Set search direction on arrow key inputs
    if (key == KEY_ARROW_RIGHT || key == KEY_ARROW_DOWN) {
        direction = 1;
    } else if (key == KEY_ARROW_LEFT || key == KEY_ARROW_UP) {
        direction = -1;
    } else {
        last_match = -1;
        direction = 1;
    }

    if (last_match == -1) direction = 1;
    int current = last_match;

    // Query scan loop
    for (int i = 0; i < state->numrows; i++) {
        current += direction;
        
        // Wrap search index
        if (current == -1) current = state->numrows - 1;
        else if (current == state->numrows) current = 0;

        erow *row = &state->row[current];
        char *match = strstr(row->render, query);
        if (match != NULL) {
            last_match = current;
            state->cy = current;
            state->cx = match - row->render; // Visual column positioning
            break;
        }
    }

    // Refresh highlights for matches on all rows
    for (int i = 0; i < state->numrows; i++) {
        highlight_update_row(state, i); // Recalculate syntax coloring first
        
        int qlen = strlen(query);
        if (qlen == 0) continue;

        // Highlight matching segments in row
        char *match = state->row[i].render;
        while ((match = strstr(match, query)) != NULL) {
            int idx = match - state->row[i].render;
            memset(&state->row[i].hl[idx], HL_MATCH, qlen);
            match += qlen;
        }
    }
}

void editor_find(EditorState *state) {
    // Preserve current cursor coordinates
    int saved_cx = state->cx;
    int saved_cy = state->cy;
    int saved_rowoff = state->rowoff;
    int saved_coloff = state->coloff;

    char *query = editor_prompt(state, "Search: %s (ESC/Enter to exit, Arrows for Next/Prev)", editor_find_callback);

    if (query != NULL) {
        free(query);
    } else {
        // Restore coordinates if search was aborted
        state->cx = saved_cx;
        state->cy = saved_cy;
        state->rowoff = saved_rowoff;
        state->coloff = saved_coloff;

        // Reset highlights
        for (int i = 0; i < state->numrows; i++) {
            highlight_update_row(state, i);
        }
    }
}
