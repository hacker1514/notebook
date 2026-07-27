#ifndef UNDO_H
#define UNDO_H

#include "editor.h"

enum UndoType {
    ACTION_INSERT = 0,
    ACTION_DELETE,
    ACTION_SPLIT,
    ACTION_JOIN
};

/**
 * Record an edit transaction to the undo history stack.
 * Automatically clears the redo history stack.
 */
void undo_record(EditorState *state, int type, int cy, int cx, const char *text, int len);

/**
 * Perform undo. Reverts the last edit transaction.
 */
void editor_undo(EditorState *state);

/**
 * Perform redo. Re-applies the last undone edit transaction.
 */
void editor_redo(EditorState *state);

/**
 * Free all memory held by the undo and redo history stacks.
 */
void undo_free_all(EditorState *state);

#endif // UNDO_H
