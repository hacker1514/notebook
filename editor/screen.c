#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "screen.h"
#include "platform.h"
#include "buffer.h"
#include "highlight.h"
#include "theme.h"

// Append buffer structure to hold screen redraw contents
struct abuf {
    char *b;
    int len;
};

#define ABUF_INIT {NULL, 0}

static void abAppend(struct abuf *ab, const char *s, int len) {
    char *new_buf = realloc(ab->b, ab->len + len);
    if (new_buf == NULL) return;
    memcpy(&new_buf[ab->len], s, len);
    ab->b = new_buf;
    ab->len += len;
}

static void abFree(struct abuf *ab) {
    free(ab->b);
}

static void screen_scroll(EditorState *state) {
    state->rx = 0;
    if (state->cy < state->numrows) {
        state->rx = editor_row_cx_to_rx(&state->row[state->cy], state->cx);
    }

    // Scroll vertically
    if (state->cy < state->rowoff) {
        state->rowoff = state->cy;
    }
    if (state->cy >= state->rowoff + state->screenrows) {
        state->rowoff = state->cy - state->screenrows + 1;
    }

    // Scroll horizontally
    if (state->rx < state->coloff) {
        state->coloff = state->rx;
    }
    if (state->rx >= state->coloff + state->screencols) {
        state->coloff = state->rx - state->screencols + 1;
    }
}

static int is_selected(EditorState *state, int y, int x) {
    if (!state->selection_active) return 0;
    
    int sy, sx, ey, ex;
    if (state->sel_sy < state->sel_ey) {
        sy = state->sel_sy; sx = state->sel_sx;
        ey = state->sel_ey; ex = state->sel_ex;
    } else if (state->sel_sy > state->sel_ey) {
        sy = state->sel_ey; sx = state->sel_ex;
        ey = state->sel_sy; ex = state->sel_sx;
    } else {
        sy = state->sel_sy; ey = state->sel_ey;
        if (state->sel_sx <= state->sel_ex) {
            sx = state->sel_sx; ex = state->sel_ex;
        } else {
            sx = state->sel_ex; ex = state->sel_sx;
        }
    }
    
    if (y > sy && y < ey) return 1;
    if (y == sy && y == ey) return (x >= sx && x < ex);
    if (y == sy && y < ey) return (x >= sx);
    if (y == ey && y > sy) return (x < ex);
    
    return 0;
}

static void screen_draw_rows(EditorState *state, struct abuf *ab) {
    EditorTheme *theme = (EditorTheme *)state->theme;

    for (int y = 0; y < state->screenrows; y++) {
        int file_row = y + state->rowoff;

        if (file_row < state->numrows) {
            int len = state->row[file_row].rsize - state->coloff;
            if (len < 0) len = 0;
            if (len > state->screencols) len = state->screencols;

            if (len > 0) {
                if (state->row[file_row].hl == NULL) {
                    char *render = &state->row[file_row].render[state->coloff];
                    for (int j = 0; j < len; j++) {
                        int select_mode = is_selected(state, file_row, j + state->coloff);
                        if (select_mode) {
                            abAppend(ab, "\x1b[7m", 4);
                        }
                        abAppend(ab, &render[j], 1);
                        if (select_mode) {
                            abAppend(ab, "\x1b[27m", 5);
                        }
                    }
                } else {
                    int current_color = -1;
                    unsigned char *hl = &state->row[file_row].hl[state->coloff];
                    char *render = &state->row[file_row].render[state->coloff];
                    
                    for (int j = 0; j < len; j++) {
                        if (hl[j] != current_color) {
                            current_color = hl[j];
                            char *color_esc = theme->normal;
                            
                            switch (current_color) {
                                case HL_COMMENT:
                                case HL_MLCOMMENT: color_esc = theme->comment; break;
                                case HL_KEYWORD1:  color_esc = theme->keyword1; break;
                                case HL_KEYWORD2:  color_esc = theme->keyword2; break;
                                case HL_STRING:    color_esc = theme->string; break;
                                case HL_NUMBER:    color_esc = theme->number; break;
                                case HL_MATCH:     color_esc = theme->match; break;
                                default:           color_esc = theme->normal; break;
                            }
                            abAppend(ab, color_esc, strlen(color_esc));
                        }
                        
                        int select_mode = is_selected(state, file_row, j + state->coloff);
                        if (select_mode) {
                            abAppend(ab, "\x1b[7m", 4); // Invert
                        }
                        
                        abAppend(ab, &render[j], 1);
                        
                        if (select_mode) {
                            abAppend(ab, "\x1b[27m", 5); // Normal
                            current_color = -1; // Force re-evaluating colors on next char
                        }
                    }
                    // Restore back to normal style at the end of the line
                    abAppend(ab, theme->normal, strlen(theme->normal));
                }
            }
        } else {
            // Outside of loaded file lines: draw tilde indicators in muted comment color
            if (state->numrows == 0 && state->filename == NULL &&
                (y == state->screenrows / 3 || y == state->screenrows / 3 + 1)) {
                
                char welcome[80];
                if (y == state->screenrows / 3) {
                    snprintf(welcome, sizeof(welcome), "Notebook Editor        : Version 0.0.1");
                } else {
                    snprintf(welcome, sizeof(welcome), "Developer              : Niranjan Kumar K");
                }
                
                int welcomelen = strlen(welcome);
                if (welcomelen > state->screencols) {
                    welcomelen = state->screencols;
                }
                
                // Base padding calculations on the longer string (41 chars) to align colons perfectly
                int padding = (state->screencols - 41) / 2;
                if (padding < 0) padding = 0;
                
                if (padding) {
                    abAppend(ab, theme->comment, strlen(theme->comment));
                    abAppend(ab, "~", 1);
                    abAppend(ab, theme->normal, strlen(theme->normal));
                    padding--;
                }
                while (padding--) {
                    abAppend(ab, " ", 1);
                }
                abAppend(ab, welcome, welcomelen);
            } else {
                abAppend(ab, theme->comment, strlen(theme->comment));
                abAppend(ab, "~", 1);
                abAppend(ab, theme->normal, strlen(theme->normal));
            }
        }

        // Clear row content to the right of the cursor
        abAppend(ab, "\033[K", 3);
        abAppend(ab, "\r\n", 2);
    }
}

static void screen_draw_statusbar(EditorState *state, struct abuf *ab) {
    EditorTheme *theme = (EditorTheme *)state->theme;

    // Apply status bar TrueColor background
    abAppend(ab, theme->status_bar, strlen(theme->status_bar));

    // 1. Mode indicator
    char mode_str[24];
    switch (state->mode) {
        case MODE_NORMAL:   snprintf(mode_str, sizeof(mode_str), " NORMAL "); break;
        case MODE_READONLY: snprintf(mode_str, sizeof(mode_str), " READONLY "); break;
        default:            snprintf(mode_str, sizeof(mode_str), " INSERT "); break;
    }

    // 2. Build buffer tabs list (representing open files)
    char tabs[128];
    int tabs_len = 0;
    tabs[0] = '\0';
    for (int i = 0; i < state->num_buffers; i++) {
        char tab_str[32];
        char *fname = state->buffers[i].filename;
        char *display_name = fname ? strrchr(fname, '/') : NULL;
        if (!display_name && fname) display_name = strrchr(fname, '\\');
        display_name = display_name ? display_name + 1 : (fname ? fname : "[No Name]");
        
        if (i == state->active_buffer) {
            snprintf(tab_str, sizeof(tab_str), "[%d:%s%s] ", i + 1, display_name, state->buffers[i].dirty ? "*" : "");
        } else {
            snprintf(tab_str, sizeof(tab_str), "%d:%s%s ", i + 1, display_name, state->buffers[i].dirty ? "*" : "");
        }
        
        if (tabs_len + strlen(tab_str) < sizeof(tabs)) {
            strcat(tabs, tab_str);
            tabs_len += strlen(tab_str);
        }
    }

    char status[256];
    int len = snprintf(status, sizeof(status), " %s | %s", mode_str, tabs);
    
    // 3. Query system time (HH:MM)
    time_t rawtime = time(NULL);
    struct tm *timeinfo = localtime(&rawtime);
    char time_str[16];
    if (timeinfo != NULL) {
        strftime(time_str, sizeof(time_str), "%H:%M", timeinfo);
    } else {
        strcpy(time_str, "--:--");
    }

    char rstatus[128];
    int rlen = snprintf(rstatus, sizeof(rstatus), " %s | %s | UTF-8 | %s | %d/%d ",
                        theme->name,
                        state->line_ending == LE_CRLF ? "CRLF" : "LF",
                        time_str,
                        state->cy + 1, state->numrows);
    
    if (len > state->screencols) len = state->screencols;
    abAppend(ab, status, len);

    // Pad space between left and right align status info
    while (len < state->screencols) {
        if (state->screencols - len == rlen) {
            // Apply text accent colors for the right status info
            abAppend(ab, theme->status_text, strlen(theme->status_text));
            abAppend(ab, rstatus, rlen);
            break;
        } else {
            abAppend(ab, " ", 1);
            len++;
        }
    }

    // Reset character properties
    abAppend(ab, "\x1b[m", 3);
    abAppend(ab, "\r\n", 2);
}

static void screen_draw_messagebar(EditorState *state, struct abuf *ab) {
    abAppend(ab, "\x1b[K", 3);

    int msglen = strlen(state->statusmsg);
    if (msglen > state->screencols) msglen = state->screencols;

    // Display message using the theme status text accent color
    if (msglen && (time(NULL) - state->statusmsg_time < 5)) {
        EditorTheme *theme = (EditorTheme *)state->theme;
        abAppend(ab, theme->status_text, strlen(theme->status_text));
        abAppend(ab, state->statusmsg, msglen);
        abAppend(ab, "\x1b[m", 3); // Reset style
    }
}

void screen_refresh(EditorState *state) {
    // Calculate scroll offset based on cursor movement
    screen_scroll(state);

    struct abuf ab = ABUF_INIT;

    // Hide cursor before rendering to prevent flickering
    abAppend(&ab, "\033[?25l", 6);
    // Home cursor
    abAppend(&ab, "\033[H", 3);

    // Apply theme normal text/background styling
    EditorTheme *theme = (EditorTheme *)state->theme;
    abAppend(&ab, theme->normal, strlen(theme->normal));

    // Draw lines of text
    screen_draw_rows(state, &ab);

    // Draw status and prompt line
    screen_draw_statusbar(state, &ab);
    screen_draw_messagebar(state, &ab);

    // Reposition cursor at current coordinates (relative to window offset)
    char buf[32];
    snprintf(buf, sizeof(buf), "\033[%d;%dH", 
             (state->cy - state->rowoff) + 1, 
             (state->rx - state->coloff) + 1);
    abAppend(&ab, buf, strlen(buf));

    // Show cursor again
    abAppend(&ab, "\033[?25h", 6);

    // Write buffer contents to screen in a single output operation
    if (ab.b != NULL) {
        fwrite(ab.b, 1, ab.len, stdout);
        fflush(stdout);
        abFree(&ab);
    }
}
