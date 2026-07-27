#ifndef HELP_H
#define HELP_H

#include "editor.h"

/**
 * Open a read-only virtual tab containing the user manual and developer attribution.
 * Triggered by ':help' command or the F1 hotkey.
 */
void editor_open_help_manual(EditorState *state);

#endif // HELP_H
