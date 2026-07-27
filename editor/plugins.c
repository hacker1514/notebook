#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "plugins.h"
#include "buffer.h"
#include "highlight.h"

#define MAX_PLUGINS 32

typedef struct {
    char *name;
    EditorPluginCallback callback;
} PluginEntry;

// Registry array for each of the event enums
static PluginEntry registry[3][MAX_PLUGINS];
static int registry_counts[3] = {0, 0, 0};

void plugins_register(EditorEvent event, const char *name, EditorPluginCallback callback) {
    int idx = (int)event;
    if (idx < 0 || idx >= 3) return;
    if (registry_counts[idx] >= MAX_PLUGINS) return;

    registry[idx][registry_counts[idx]].name = (char *)name;
    registry[idx][registry_counts[idx]].callback = callback;
    registry_counts[idx]++;
}

void plugins_trigger(EditorState *state, EditorEvent event, void *event_data) {
    int idx = (int)event;
    if (idx < 0 || idx >= 3) return;
    
    for (int i = 0; i < registry_counts[idx]; i++) {
        registry[idx][i].callback(state, event_data);
    }
}

// Built-in Plugin 1: Bracket Auto-Close
static void plugin_autoclose_brackets(EditorState *state, void *event_data) {
    int *key_ptr = (int *)event_data;
    int c = *key_ptr;
    char close_char = '\0';

    if (c == '(')      close_char = ')';
    else if (c == '[') close_char = ']';
    else if (c == '{') close_char = '}';
    else if (c == '"') close_char = '"';
    else if (c == '\'') close_char = '\'';

    if (close_char != '\0' && state->cy < state->numrows) {
        // Insert closing character immediately after cursor position
        editor_row_insert_char(&state->row[state->cy], state->cx, close_char);
        highlight_update_row(state, state->cy);
    }
}

// Built-in Plugin 2: Word Count reporter
static void plugin_word_count(EditorState *state, void *event_data) {
    (void)event_data;
    int words = 0;
    
    for (int i = 0; i < state->numrows; i++) {
        erow *row = &state->row[i];
        int in_word = 0;
        for (int j = 0; j < row->size; j++) {
            if (isspace((unsigned char)row->chars[j])) {
                in_word = 0;
            } else if (!in_word) {
                words++;
                in_word = 1;
            }
        }
    }
    
    editor_set_status_message(state, "File Saved! Word Count: %d words", words);
}

void plugins_init(void) {
    plugins_register(EVENT_ON_KEY, "AutoCloseBrackets", plugin_autoclose_brackets);
    plugins_register(EVENT_ON_SAVE, "WordCount", plugin_word_count);
}
