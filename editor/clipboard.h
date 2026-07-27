#ifndef CLIPBOARD_H
#define CLIPBOARD_H

#include "editor.h"

/**
 * Copy the current selected text range into the global clipboard buffer.
 * Automatically clears the active selection range upon completion.
 */
void editor_copy(EditorState *state);

/**
 * Paste the contents of the global clipboard buffer at the current cursor coordinates.
 * Supports multi-line pasting.
 */
void editor_paste(EditorState *state);

/**
 * Select the entire contents of the current file buffer (Ctrl+A).
 */
void editor_select_all(EditorState *state);

/**
 * Free any globally allocated clipboard buffer memory on editor exit.
 */
void clipboard_free_global(void);

#endif // CLIPBOARD_H
