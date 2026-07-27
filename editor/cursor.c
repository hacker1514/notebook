#include <stddef.h>
#include "cursor.h"
#include "keymap.h"

void cursor_move(EditorState *state, int key) {
    erow *row = (state->cy >= state->numrows) ? NULL : &state->row[state->cy];

    switch (key) {
        case KEY_ARROW_LEFT:
            if (state->cx > 0) {
                state->cx--;
            } else if (state->cy > 0) {
                // Wrap to end of previous line
                state->cy--;
                state->cx = state->row[state->cy].size;
            }
            break;

        case KEY_ARROW_RIGHT:
            if (row && state->cx < row->size) {
                state->cx++;
            } else if (row && state->cx == row->size && state->cy < state->numrows - 1) {
                // Wrap to start of next line
                state->cy++;
                state->cx = 0;
            }
            break;

        case KEY_ARROW_UP:
            if (state->cy > 0) {
                state->cy--;
            }
            break;

        case KEY_ARROW_DOWN:
            if (state->cy < state->numrows - 1) {
                state->cy++;
            }
            break;

        case KEY_PAGE_UP: {
            state->cy -= state->screenrows;
            if (state->cy < 0) state->cy = 0;
            break;
        }

        case KEY_PAGE_DOWN: {
            state->cy += state->screenrows;
            if (state->cy >= state->numrows) state->cy = state->numrows - 1;
            if (state->cy < 0) state->cy = 0;
            break;
        }

        case KEY_HOME:
            state->cx = 0;
            break;

        case KEY_END:
            if (row) {
                state->cx = row->size;
            }
            break;
    }

    // Clamp cursor column to the length of the current row (in case of vertical navigation)
    row = (state->cy >= state->numrows) ? NULL : &state->row[state->cy];
    int rowlen = row ? row->size : 0;
    if (state->cx > rowlen) {
        state->cx = rowlen;
    }
}
