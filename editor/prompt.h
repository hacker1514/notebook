#ifndef PROMPT_H
#define PROMPT_H

#include "editor.h"

/**
 * Display a prompt in the status bar and enter a keyboard input loop.
 * The prompt format should contain a %s where the user's typed string is rendered.
 * Returns a dynamically allocated string on Enter, or NULL on Escape.
 */
char *editor_prompt(EditorState *state, const char *prompt_format, void (*callback)(EditorState *, const char *, int));

#endif // PROMPT_H
