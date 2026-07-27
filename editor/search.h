#ifndef SEARCH_H
#define SEARCH_H

#include "editor.h"

/**
 * Open the incremental search prompt (Ctrl+F).
 * Updates cursor dynamically on matching characters and highlights all search results.
 */
void editor_find(EditorState *state);

#endif // SEARCH_H
