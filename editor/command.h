#ifndef COMMAND_H
#define COMMAND_H

#include "editor.h"

/**
 * Enter Vim-style Command Mode (prompted by ':').
 * Supports commands: help, w, q, wq, q!
 */
void editor_enter_command_mode(EditorState *state);

#endif // COMMAND_H
