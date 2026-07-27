#ifndef EDITOR_H
#define EDITOR_H

#include <stdbool.h>
#include <time.h>

typedef struct erow {
    int size;           // Raw character count of line
    char *chars;        // Pointer to line characters
    int rsize;          // Character count for rendering (with expanded tabs)
    char *render;       // Pointer to rendering characters
    unsigned char *hl;  // Array of highlight types for each character in render
    int hl_open_comment; // Flag indicating if line ends with an unclosed multiline comment
} erow;

typedef struct UndoAction {
    int type;            // ACTION_INSERT, ACTION_DELETE, ACTION_SPLIT, ACTION_JOIN
    int cy, cx;          // Location of edit
    char *text;          // Copy of string content (or char value)
    int len;             // String length
    struct UndoAction *next;
} UndoAction;

enum editorMode {
    MODE_INSERT = 0,
    MODE_NORMAL,
    MODE_READONLY
};

enum lineEndingType {
    LE_LF = 0,
    LE_CRLF
};

typedef struct {
    char *filename;      // Path to loaded file
    int numrows;         // Total rows in buffer
    erow *row;           // Line rows array
    int row_cap;         // Pre-allocated rows capacity (for optimization)
    int cx, cy;          // Cursor coordinates
    int rx;              // Render column index
    int rowoff;          // Scroll offsets
    int coloff;
    int dirty;           // Dirty indicator
    void *syntax;        // Selected syntax configuration
    UndoAction *undo_stack; // History stacks
    UndoAction *redo_stack;
    int line_ending;     // CRLF vs LF
    int sel_sx, sel_sy;  // Selection start (col, row)
    int sel_ex, sel_ey;  // Selection end (col, row)
    int selection_active; // 1 if selection is active, 0 otherwise
} EditorBuffer;

typedef struct {
    int screenrows;      // Screen rows available for editing
    int screencols;      // Screen columns available for editing
    bool quit;           // Flag indicating request to quit editor
    
    // Multi-buffer state
    int active_buffer;   // Index of active buffer in buffers array
    int num_buffers;     // Total open buffers
    EditorBuffer *buffers; // Array of open buffers
    
    // Active buffer duplicates (for fast swapping)
    int cx, cy;
    int rx;
    int rowoff;
    int coloff;
    int numrows;
    erow *row;
    int row_cap;         // Tracked rows array capacity
    int dirty;
    char *filename;
    int line_ending;
    void *syntax;
    UndoAction *undo_stack;
    UndoAction *redo_stack;
    int sel_sx, sel_sy;  // Active selection start (col, row)
    int sel_ex, sel_ey;  // Active selection end (col, row)
    int selection_active; // Active selection state
    
    // Status message state
    char statusmsg[80];
    time_t statusmsg_time;
    int mode;
    void *theme;
} EditorState;

/**
 * Initialize editor state.
 * Returns true on success, false on failure.
 */
bool editor_init(EditorState *state);

/**
 * Clean up resources allocated for the editor.
 */
void editor_cleanup(EditorState *state);

/**
 * Update the status bar with a formatted notification message.
 */
void editor_set_status_message(EditorState *state, const char *fmt, ...);

/**
 * Create a new empty text buffer (Ctrl+N).
 */
void buffer_new(EditorState *state);

/**
 * Open a file in a new text buffer, or switch to it if already open (Ctrl+O).
 */
bool buffer_open(EditorState *state, const char *filename);

/**
 * Close the current active text buffer (Ctrl+W).
 */
void buffer_close(EditorState *state);

/**
 * Cycle buffers (Tab/Shift+Tab in Normal mode). Dir: 1 = next, -1 = prev.
 */
void buffer_cycle(EditorState *state, int dir);

/**
 * Run the editor's main loop.
 */
void editor_run(EditorState *state);

#endif // EDITOR_H
