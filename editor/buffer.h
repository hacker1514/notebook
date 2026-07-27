#ifndef BUFFER_H
#define BUFFER_H

#include "editor.h"

// Tab stop configuration (default: 4 spaces per tab)
#define NOTEBOOK_TAB_STOP 4

/**
 * Convert a raw character index (cx) to its visual rendering index (rx),
 * expanding tab stops correctly.
 */
int editor_row_cx_to_rx(erow *row, int cx);

/**
 * Refresh the render buffer for an erow struct (expanding tabs to spaces).
 */
void editor_update_row(erow *row);

/**
 * Insert a line of text of a given length at a specific index in the editor's rows.
 */
void editor_insert_row(EditorState *state, int at, const char *s, size_t len);

/**
 * Free memory held by a text row.
 */
void editor_free_row(erow *row);

/**
 * Insert a character 'c' at position 'at' in the specified row.
 */
void editor_row_insert_char(erow *row, int at, int c);

/**
 * Delete a character at position 'at' in the specified row.
 */
void editor_row_delete_char(erow *row, int at);

/**
 * Append a string of length 'len' to the end of the specified row.
 */
void editor_row_append_string(erow *row, const char *s, size_t len);

/**
 * Delete a row from the editor state at index 'at'.
 */
void editor_delete_row(EditorState *state, int at);

/**
 * High-level operation: Insert a character 'c' at the current cursor position.
 */
void editor_insert_char(EditorState *state, int c);

/**
 * High-level operation: Delete the character under (Delete) or before (Backspace) the cursor.
 */
void editor_delete_char(EditorState *state);

/**
 * High-level operation: Split the current row on Enter/Return.
 */
void editor_insert_newline(EditorState *state);

#endif // BUFFER_H
