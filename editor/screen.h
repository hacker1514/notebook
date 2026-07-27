#ifndef SCREEN_H
#define SCREEN_H

#include "editor.h"

/**
 * Refresh and redraw the entire terminal screen.
 * Implements a double-buffering mechanism to prevent flickering.
 */
void screen_refresh(EditorState *state);

#endif // SCREEN_H
