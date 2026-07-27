#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file.h"
#include "buffer.h"
#include "prompt.h"
#include "terminal.h"
#include "syntax.h"
#include "highlight.h"

static char *portable_strdup(const char *s) {
    if (s == NULL) return NULL;
    size_t len = strlen(s) + 1;
    char *dup = malloc(len);
    if (dup) {
        memcpy(dup, s, len);
    }
    return dup;
}

bool file_open(EditorState *state, const char *filename) {
    // Save filename to state first, so that missing files can still be created/edited
    free(state->filename);
    state->filename = portable_strdup(filename);

    // Open in binary mode to read exact bytes portably across platforms
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) return false;

    // Determine the size of the file
    if (fseek(fp, 0, SEEK_END) == -1) {
        fclose(fp);
        return false;
    }
    long size = ftell(fp);
    if (size == -1) {
        fclose(fp);
        return false;
    }
    rewind(fp);

    if (size == 0) {
        fclose(fp);
        syntax_select_by_filename(state);
        return true;
    }

    // Allocate memory to hold file contents
    char *buf = malloc(size + 1);
    if (buf == NULL) {
        fclose(fp);
        return false;
    }

    size_t bytes_read = fread(buf, 1, size, fp);
    fclose(fp);

    if (bytes_read == 0) {
        free(buf);
        syntax_select_by_filename(state);
        return true;
    }
    buf[bytes_read] = '\0';

    // Parse content line by line and detect line endings
    char *line_start = buf;
    char *p = buf;
    char *end = buf + bytes_read;
    int lf_count = 0;
    int crlf_count = 0;

    while (p < end) {
        if (*p == '\r' || *p == '\n') {
            size_t len = p - line_start;
            editor_insert_row(state, state->numrows, line_start, len);

            if (*p == '\r' && (p + 1 < end) && *(p + 1) == '\n') {
                crlf_count++;
                p += 2; // Skip both '\r' and '\n'
            } else {
                lf_count++;
                p += 1; // Skip single newline or carriage return
            }
            line_start = p;
        } else {
            p++;
        }
    }

    // Insert last line if there is no trailing newline
    if (line_start < end) {
        size_t len = end - line_start;
        editor_insert_row(state, state->numrows, line_start, len);
    }

    free(buf);

    // Register detected line ending format
    if (crlf_count > lf_count) {
        state->line_ending = LE_CRLF;
    } else if (lf_count > 0 || crlf_count > 0) {
        state->line_ending = LE_LF;
    }

    // Dynamic syntax selection and row highlighting trigger
    syntax_select_by_filename(state);
    for (int i = 0; i < state->numrows; i++) {
        highlight_update_row(state, i);
    }

    return true;
}

char *editor_rows_to_string(EditorState *state, int *buflen) {
    int totlen = 0;
    for (int i = 0; i < state->numrows; i++) {
        totlen += state->row[i].size + 1; // +1 for '\n'
    }
    *buflen = totlen;

    char *buf = malloc(totlen + 1);
    if (buf == NULL) return NULL;
    
    char *p = buf;
    for (int i = 0; i < state->numrows; i++) {
        memcpy(p, state->row[i].chars, state->row[i].size);
        p += state->row[i].size;
        *p = '\n';
        p++;
    }
    *p = '\0';
    return buf;
}

void editor_save(EditorState *state) {
    if (state->filename == NULL) {
        state->filename = editor_prompt(state, "Save as: %s (ESC to cancel)", NULL);
        if (state->filename == NULL) {
            editor_set_status_message(state, "Save aborted");
            return;
        }
        
        // Update the terminal title with the new filename and trigger syntax highlighting detection
        char title[256];
        snprintf(title, sizeof(title), "Notebook - %s", state->filename);
        terminal_set_title(title);
        
        syntax_select_by_filename(state);
        for (int i = 0; i < state->numrows; i++) {
            highlight_update_row(state, i);
        }
    }

    int len;
    char *buf = editor_rows_to_string(state, &len);
    if (buf == NULL) {
        editor_set_status_message(state, "Save failed: Out of memory");
        return;
    }

    // Open file in binary write mode
    FILE *fp = fopen(state->filename, "wb");
    if (fp != NULL) {
        if (fwrite(buf, 1, len, fp) == (size_t)len) {
            fclose(fp);
            free(buf);
            state->dirty = 0;
            editor_set_status_message(state, "%d bytes written to disk", len);
            return;
        }
        fclose(fp);
    }

    free(buf);
    editor_set_status_message(state, "Save failed: I/O error");
}
