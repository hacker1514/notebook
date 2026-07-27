#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "prompt.h"
#include "platform.h"
#include "keymap.h"
#include "screen.h"

char *editor_prompt(EditorState *state, const char *prompt_format, void (*callback)(EditorState *, const char *, int)) {
    size_t bufsize = 128;
    char *buf = malloc(bufsize);
    if (buf == NULL) return NULL;

    size_t buflen = 0;
    buf[0] = '\0';

    while (true) {
        editor_set_status_message(state, prompt_format, buf);
        screen_refresh(state);

        int c = platform_get_key();
        
        // Handle Backspace / Delete
        if (c == KEY_BACKSPACE || c == KEY_DELETE || c == CTRL_KEY('h')) {
            if (buflen > 0) {
                buf[--buflen] = '\0';
            }
        } 
        // Cancel prompt on Escape
        else if (c == KEY_ESCAPE) {
            editor_set_status_message(state, "");
            if (callback) callback(state, buf, c);
            free(buf);
            return NULL;
        } 
        // Complete input on Enter
        else if (c == KEY_ENTER || c == KEY_CARRIAGE_RETURN) {
            if (buflen > 0) {
                editor_set_status_message(state, "");
                if (callback) callback(state, buf, c);
                return buf;
            }
        } 
        // Append printable characters
        else if (c >= 32 && c < 127) {
            if (buflen + 1 >= bufsize) {
                bufsize *= 2;
                char *new_buf = realloc(buf, bufsize);
                if (new_buf == NULL) {
                    free(buf);
                    return NULL;
                }
                buf = new_buf;
            }
            buf[buflen++] = c;
            buf[buflen] = '\0';
        }

        // Fire callback on every character change/navigation key
        if (callback) callback(state, buf, c);
    }
}
