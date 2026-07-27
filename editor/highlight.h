#ifndef HIGHLIGHT_H
#define HIGHLIGHT_H

#include "editor.h"

// Highlight type enums
enum editorHighlight {
    HL_NORMAL = 0,
    HL_COMMENT,
    HL_MLCOMMENT,       // Multiline comment
    HL_KEYWORD1,        // Normal keywords
    HL_KEYWORD2,        // Type keywords (suffixed with '|')
    HL_STRING,          // Double/Single quoted strings
    HL_NUMBER,          // Alphanumeric numbers
    HL_MATCH            // Search match highlights
};

/**
 * Regenerate syntax coloring tokens for row 'row_idx'.
 * Propagates multiline comment changes to subsequent rows automatically.
 */
void highlight_update_row(EditorState *state, int row_idx);

#endif // HIGHLIGHT_H
