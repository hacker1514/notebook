#ifndef PLUGINS_H
#define PLUGINS_H

#include "editor.h"

typedef enum {
    EVENT_ON_KEY,        // Fired after a character is inserted
    EVENT_ON_SAVE,       // Fired after a file is saved
    EVENT_ON_LOAD        // Fired after a file is loaded
} EditorEvent;

typedef void (*EditorPluginCallback)(EditorState *state, void *event_data);

/**
 * Register a plugin callback function to an editor event hook.
 */
void plugins_register(EditorEvent event, const char *name, EditorPluginCallback callback);

/**
 * Trigger all registered plugins for the specified editor event.
 */
void plugins_trigger(EditorState *state, EditorEvent event, void *event_data);

/**
 * Initialize built-in plugins (Bracket Auto-Closing and Word Counter).
 */
void plugins_init(void);

#endif // PLUGINS_H
