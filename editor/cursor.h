#ifndef CURSOR_H
#define CURSOR_H

#include "editor.h"

/**
 * Handle cursor movement keys (arrows, Home/End, Page Up/Down)
 * while respecting row boundaries and clamping correctly.
 */
void cursor_move(EditorState *state, int key);

#endif // CURSOR_H
