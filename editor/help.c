#include <stdlib.h>
#include <string.h>
#include "help.h"
#include "buffer.h"

static char *help_strdup(const char *s) {
    size_t len = strlen(s) + 1;
    char *dup = malloc(len);
    if (dup) memcpy(dup, s, len);
    return dup;
}

static const char *help_lines[] = {
    "========================================================================",
    "                      NOTEBOOK EDITOR USER MANUAL                  ",
    "========================================================================",
    " Developer : Niranjan Kumar K                                           ",
    " Version   : 0.0.1                                                      ",
    "========================================================================",
    "",
    " Notebook is a modern, high-performance terminal text editor designed",
    " with clean C17 code, full TrueColor syntax themes, and multi-file tabs.",
    "",
    "------------------------------------------------------------------------",
    " 1. SHORTCUTS & CONTROLS",
    "------------------------------------------------------------------------",
    "   Ctrl+N       : Create a new empty file tab.",
    "   Ctrl+O       : Open a file in a new tab (prompts for file name).",
    "   Ctrl+W       : Close the active file tab.",
    "   Ctrl+S       : Save the current buffer to disk.",
    "   Ctrl+F       : Open incremental search (Arrows cycle, Enter/ESC exit).",
    "   Ctrl+Z       : Undo the last edit.",
    "   Ctrl+Y       : Redo the last undone edit.",
    "   Ctrl+A       : Select all text in the file.",
    "   Ctrl+C       : Copy selected text to clipboard.",
    "   Ctrl+V       : Paste text from clipboard.",
    "   F5           : Cycle color themes (One Dark, Dracula, Gruvbox, Nord).",
    "   F1           : Open this Help Manual buffer.",
    "   Ctrl+Q       : Quit editor (warns if file has unsaved changes).",
    "",
    "------------------------------------------------------------------------",
    " 2. MODES OF OPERATION",
    "------------------------------------------------------------------------",
    " Notebook operates in three modes: Insert, Normal, and Command.",
    "",
    " * INSERT MODE (Default)",
    "   Type text directly. Press Escape to switch to Normal Mode.",
    "",
    " * NORMAL MODE (Navigation & Operations)",
    "   Keys perform commands rather than inserting text:",
    "     h / j / k / l : Move cursor Left / Down / Up / Right.",
    "     i             : Return to Insert Mode.",
    "     u             : Perform Undo.",
    "     r             : Perform Redo.",
    "     Tab           : Switch to next open buffer tab.",
    "     Shift+Tab     : Switch to previous open buffer tab.",
    "     : (colon)     : Switch to Command Mode.",
    "",
    " * COMMAND MODE",
    "   A prompt ':' will appear in the status line. Type one of these commands:",
    "     help          : Open this help manual in a new buffer.",
    "     w             : Save the active buffer.",
    "     q             : Close the active buffer / quit.",
    "     wq            : Save and close.",
    "     q!            : Force close ignoring modifications.",
    "   Press Enter to run the command, or ESC to return to Normal Mode.",
    "========================================================================"
};

#define HELP_LINES_COUNT (sizeof(help_lines) / sizeof(help_lines[0]))

void editor_open_help_manual(EditorState *state) {
    // Open a new blank buffer
    buffer_new(state);

    // Set buffer metadata
    state->filename = help_strdup("[Help Manual]");

    // Fill buffer line-by-line
    for (size_t i = 0; i < HELP_LINES_COUNT; i++) {
        editor_insert_row(state, state->numrows, help_lines[i], strlen(help_lines[i]));
    }

    // Set dirty state to 0 so the help buffer can close without save warnings
    state->dirty = 0;

    // Reset cursor position to top of document
    state->cx = 0;
    state->cy = 0;
    state->rowoff = 0;
    state->coloff = 0;

    editor_set_status_message(state, "Loaded Help Manual. Press Ctrl+W or type :q to close.");
}
