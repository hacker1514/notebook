#include <string.h>
#include "theme.h"

// Theme database containing TrueColor escape properties
static EditorTheme themes_db[] = {
    {
        "onedark",
        "\x1b[38;2;171;178;191m", // Normal text
        "\x1b[38;2;92;99;112m",   // Comment
        "\x1b[38;2;198;120;221m", // Keyword 1
        "\x1b[38;2;97;175;239m",  // Keyword 2
        "\x1b[38;2;152;195;121m", // String
        "\x1b[38;2;209;154;102m", // Number
        "\x1b[37;48;2;97;175;239m", // Match (white on blue)
        "\x1b[38;2;171;178;191;48;2;44;50;60m", // Status bar background
        "\x1b[38;2;224;108;117m"  // Status bar accent
    },
    {
        "dracula",
        "\x1b[38;2;248;248;242m", // Normal text
        "\x1b[38;2;98;114;164m",   // Comment
        "\x1b[38;2;255;121;198m", // Keyword 1
        "\x1b[38;2;139;233;253m", // Keyword 2
        "\x1b[38;2;241;250;140m", // String
        "\x1b[38;2;189;147;249m", // Number
        "\x1b[30;48;2;80;250;123m", // Match (dark on green)
        "\x1b[38;2;248;248;242;48;2;98;114;164m", // Status bar background
        "\x1b[38;2;80;250;123m"   // Status bar accent
    },
    {
        "gruvbox",
        "\x1b[38;2;235;219;178m", // Normal text
        "\x1b[38;2;146;131;116m", // Comment
        "\x1b[38;2;251;73;52m",   // Keyword 1
        "\x1b[38;2;142;192;124m", // Keyword 2
        "\x1b[38;2;184;187;38m",  // String
        "\x1b[38;2;211;134;155m", // Number
        "\x1b[30;48;2;250;189;47m", // Match (dark on gold)
        "\x1b[38;2;235;219;178;48;2;80;73;69m", // Status bar background
        "\x1b[38;2;250;189;47m"   // Status bar accent
    },
    {
        "nord",
        "\x1b[38;2;216;222;233m", // Normal text
        "\x1b[38;2;76;86;106m",   // Comment
        "\x1b[38;2;129;161;193m", // Keyword 1
        "\x1b[38;2;143;188;187m", // Keyword 2
        "\x1b[38;2;163;190;140m", // String
        "\x1b[38;2;180;142;173m", // Number
        "\x1b[30;48;2;136;192;208m", // Match (dark on teal)
        "\x1b[38;2;216;222;233;48;2;67;76;94m", // Status bar background
        "\x1b[38;2;136;192;208m"  // Status bar accent
    },
    {
        "solarizedlight",
        "\x1b[38;2;88;110;117m",  // Normal text
        "\x1b[38;2;147;161;161m", // Comment
        "\x1b[38;2;133;153;0m",   // Keyword 1
        "\x1b[38;2;38;139;210m",  // Keyword 2
        "\x1b[38;2;42;161;152m",  // String
        "\x1b[38;2;203;75;22m",   // Number
        "\x1b[37;48;2;181;137;0m",  // Match (white on yellow)
        "\x1b[38;2;88;110;117;48;2;238;232;213m", // Status bar background
        "\x1b[38;2;203;75;22m"   // Status bar accent
    },
    {
        "hacker",
        "\x1b[38;2;0;255;0;48;2;0;0;0m",  // Normal text (lime green on black)
        "\x1b[38;2;0;130;0;48;2;0;0;0m",    // Comment (dark green on black)
        "\x1b[38;2;0;255;0;1;48;2;0;0;0m",   // Keyword 1 (bold lime green on black)
        "\x1b[38;2;100;255;100;48;2;0;0;0m", // Keyword 2 (light green on black)
        "\x1b[38;2;150;255;150;48;2;0;0;0m", // String (very light green on black)
        "\x1b[38;2;0;200;0;48;2;0;0;0m",    // Number (muted green on black)
        "\x1b[30;48;2;0;255;0m",             // Match (black on bright lime green)
        "\x1b[30;48;2;0;200;0m",             // Status bar (black on green)
        "\x1b[38;2;0;255;0;48;2;0;100;0m"    // Status bar accent (lime on dark green)
    }
};

#define THEMES_COUNT (sizeof(themes_db) / sizeof(themes_db[0]))

void theme_select(EditorState *state, const char *theme_name) {
    for (unsigned int i = 0; i < THEMES_COUNT; i++) {
        if (strcmp(themes_db[i].name, theme_name) == 0) {
            state->theme = &themes_db[i];
            return;
        }
    }
    // Fallback default theme is One Dark
    state->theme = &themes_db[0];
}

void theme_cycle(EditorState *state) {
    EditorTheme *current = (EditorTheme *)state->theme;
    unsigned int current_idx = 0;

    if (current != NULL) {
        for (unsigned int i = 0; i < THEMES_COUNT; i++) {
            if (strcmp(themes_db[i].name, current->name) == 0) {
                current_idx = i;
                break;
            }
        }
    }

    unsigned int next_idx = (current_idx + 1) % THEMES_COUNT;
    state->theme = &themes_db[next_idx];
    editor_set_status_message(state, "Active Theme: %s", themes_db[next_idx].name);
}
