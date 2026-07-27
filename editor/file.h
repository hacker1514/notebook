#ifndef FILE_H
#define FILE_H

#include "editor.h"

/**
 * Open a file and load its contents into the editor state buffer.
 * Automatically parses CRLF (\r\n), LF (\n), and CR (\r) line endings.
 * Returns true on success, false on failure (e.g. file not found).
 */
bool file_open(EditorState *state, const char *filename);

/**
 * Serialize the rows buffer and write it to disk.
 * Prompts for a filename if none is registered.
 */
void editor_save(EditorState *state);

#endif // FILE_H
