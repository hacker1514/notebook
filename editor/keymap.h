#ifndef KEYMAP_H
#define KEYMAP_H

// Unified key definitions for the Notebook editor.
// ASCII values 0-255 represent standard printable characters and standard control codes.
// Values 1000+ represent special keys and control combinations.
enum editorKey {
    KEY_NONE = 0,

    // Navigation keys
    KEY_ARROW_LEFT = 1000,
    KEY_ARROW_RIGHT,
    KEY_ARROW_UP,
    KEY_ARROW_DOWN,
    KEY_PAGE_UP,
    KEY_PAGE_DOWN,
    KEY_HOME,
    KEY_END,
    KEY_DELETE,
    KEY_INSERT,

    // Function keys
    KEY_F1,
    KEY_F2,
    KEY_F3,
    KEY_F4,
    KEY_F5,
    KEY_F6,
    KEY_F7,
    KEY_F8,
    KEY_F9,
    KEY_F10,
    KEY_F11,
    KEY_F12,

    // Ctrl + navigation/special keys
    KEY_CTRL_LEFT,
    KEY_CTRL_RIGHT,
    KEY_CTRL_UP,
    KEY_CTRL_DOWN,
    KEY_CTRL_HOME,
    KEY_CTRL_END,

    // Window resize event
    KEY_RESIZE,

    // Standard control key mappings for readability
    KEY_BACKSPACE = 127,
    KEY_ESCAPE = 27,
    KEY_TAB = 9,
    KEY_SHIFT_TAB = 350, // Custom unique code
    KEY_ENTER = 10,
    KEY_CARRIAGE_RETURN = 13
};

// Bitwise helper to define control key shortcuts (Ctrl+key)
#define CTRL_KEY(k) ((k) & 0x1f)

#endif // KEYMAP_H
