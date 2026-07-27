#ifndef THEME_H
#define THEME_H

#include "editor.h"

typedef struct {
    char *name;          // Visual display name
    char *normal;        // TrueColor escape string for normal text
    char *comment;       // TrueColor escape string for single/multi comments
    char *keyword1;      // TrueColor escape string for keywords
    char *keyword2;      // TrueColor escape string for types
    char *string;        // TrueColor escape string for strings
    char *number;        // TrueColor escape string for numbers
    char *match;         // TrueColor escape string for search matches (foreground + background)
    char *status_bar;    // TrueColor escape string for status bar styling
    char *status_text;   // TrueColor escape string for status bar accent markers
} EditorTheme;

/**
 * Change the editor theme to the specified name.
 * Defaults to "onedark" if theme_name is invalid/not found.
 */
void theme_select(EditorState *state, const char *theme_name);

/**
 * Cycle to the next theme in the registry database.
 */
void theme_cycle(EditorState *state);

#endif // THEME_H
