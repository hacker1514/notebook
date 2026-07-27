#ifndef SYNTAX_H
#define SYNTAX_H

#include "editor.h"

// Highlight flag configurations
#define HL_HIGHLIGHT_NUMBERS (1 << 0)
#define HL_HIGHLIGHT_STRINGS (1 << 1)

typedef struct {
    char *filetype;
    char **filematch;              // Array of file glob/extensions (NULL terminated)
    char **keywords;               // Array of language keywords (NULL terminated, types suffixed with '|')
    char *singleline_comment_start;// e.g. "//"
    char *multiline_comment_start; // e.g. "/*"
    char *multiline_comment_end;   // e.g. "*/"
    int flags;
} editorSyntax;

/**
 * Scan the syntax database and assign the matching language structure 
 * to state->syntax based on the opened filename extension.
 */
void syntax_select_by_filename(EditorState *state);

/**
 * Retrieve the current database array of editorSyntax configurations.
 */
editorSyntax *syntax_get_db(int *db_size);

#endif // SYNTAX_H
