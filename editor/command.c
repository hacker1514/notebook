#include <stdlib.h>
#include <string.h>
#include "command.h"
#include "prompt.h"
#include "file.h"
#include "help.h"

void editor_enter_command_mode(EditorState *state) {
    char *cmd = editor_prompt(state, ":%s", NULL);
    if (cmd == NULL) {
        editor_set_status_message(state, "Command cancelled");
        return;
    }

    if (strcmp(cmd, "help") == 0) {
        editor_open_help_manual(state);
    } else if (strcmp(cmd, "w") == 0) {
        editor_save(state);
    } else if (strcmp(cmd, "q") == 0) {
        buffer_close(state);
    } else if (strcmp(cmd, "wq") == 0) {
        editor_save(state);
        buffer_close(state);
    } else if (strcmp(cmd, "q!") == 0) {
        // Temporarily clear dirty flag to force close without warning prompt
        state->dirty = 0;
        buffer_close(state);
    } else {
        editor_set_status_message(state, "Error: Unknown command '%s'", cmd);
    }

    free(cmd);
}
