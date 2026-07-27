#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "highlight.h"
#include "syntax.h"

static int is_separator(int c) {
    return isspace(c) || c == '\0' || strchr(",.()+-/*=~%<>;[]&|{}", c) != NULL;
}

void highlight_update_row(EditorState *state, int row_idx) {
    if (row_idx < 0 || row_idx >= state->numrows) return;
    erow *row = &state->row[row_idx];

    // Allocate/reallocate the highlights array to match the visual render length
    unsigned char *new_hl = realloc(row->hl, row->rsize);
    if (row->rsize > 0 && new_hl == NULL) return;
    row->hl = new_hl;

    // Default to HL_NORMAL
    memset(row->hl, HL_NORMAL, row->rsize);

    // Retrieve syntax definition
    editorSyntax *s = (editorSyntax *)state->syntax;
    if (s == NULL) return;

    char **keywords = s->keywords;
    char *scs = s->singleline_comment_start;
    char *mcs = s->multiline_comment_start;
    char *mce = s->multiline_comment_end;

    int scs_len = scs ? strlen(scs) : 0;
    int mcs_len = mcs ? strlen(mcs) : 0;
    int mce_len = mce ? strlen(mce) : 0;

    // Multiline comment state inherits from previous row
    int in_comment = (row_idx > 0) ? state->row[row_idx - 1].hl_open_comment : 0;
    int in_string = 0; // Tracks string quote delimiter ('"' or '\'')

    int i = 0;
    int prev_sep = 1; // Tracks if previous char was a word separator

    while (i < row->rsize) {
        char c = row->render[i];
        unsigned char prev_hl = (i > 0) ? row->hl[i - 1] : HL_NORMAL;

        // 1. Handle Multiline Comment block
        if (mcs_len && mce_len && !in_string) {
            if (in_comment) {
                row->hl[i] = HL_MLCOMMENT;
                if (i + mce_len <= row->rsize && strncmp(&row->render[i], mce, mce_len) == 0) {
                    memset(&row->hl[i], HL_MLCOMMENT, mce_len);
                    i += mce_len;
                    in_comment = 0;
                    prev_sep = 1;
                    continue;
                } else {
                    i++;
                    continue;
                }
            } else if (i + mcs_len <= row->rsize && strncmp(&row->render[i], mcs, mcs_len) == 0) {
                memset(&row->hl[i], HL_MLCOMMENT, mcs_len);
                i += mcs_len;
                in_comment = 1;
                continue;
            }
        }

        // 2. Handle Single line Comment
        if (scs_len && !in_string && !in_comment) {
            if (i + scs_len <= row->rsize && strncmp(&row->render[i], scs, scs_len) == 0) {
                memset(&row->hl[i], HL_COMMENT, row->rsize - i);
                break;
            }
        }

        // 3. Handle Strings
        if (s->flags & HL_HIGHLIGHT_STRINGS) {
            if (in_string) {
                row->hl[i] = HL_STRING;
                // Handle escape backslashes e.g. \"
                if (c == '\\' && i + 1 < row->rsize) {
                    row->hl[i + 1] = HL_STRING;
                    i += 2;
                    continue;
                }
                if (c == in_string) in_string = 0;
                i++;
                prev_sep = 1;
                continue;
            } else {
                if (c == '"' || c == '\'') {
                    in_string = c;
                    row->hl[i] = HL_STRING;
                    i++;
                    continue;
                }
            }
        }

        // 4. Handle Numbers
        if (s->flags & HL_HIGHLIGHT_NUMBERS) {
            if ((isdigit(c) && (prev_sep || prev_hl == HL_NUMBER)) ||
                (c == '.' && prev_hl == HL_NUMBER)) {
                row->hl[i] = HL_NUMBER;
                i++;
                prev_sep = 0;
                continue;
            }
        }

        // 5. Handle Keywords (only if preceded by a separator)
        if (prev_sep && keywords) {
            int j = 0;
            int matched = 0;
            while (keywords[j]) {
                char *kw = keywords[j];
                int kwlen = strlen(kw);
                int is_type = (kw[kwlen - 1] == '|');
                int real_kwlen = is_type ? kwlen - 1 : kwlen;

                if (i + real_kwlen <= row->rsize && strncmp(&row->render[i], kw, real_kwlen) == 0 &&
                    is_separator(row->render[i + real_kwlen])) {
                    memset(&row->hl[i], is_type ? HL_KEYWORD2 : HL_KEYWORD1, real_kwlen);
                    i += real_kwlen;
                    matched = 1;
                    break;
                }
                j++;
            }
            if (matched) {
                prev_sep = 0;
                continue;
            }
        }

        prev_sep = is_separator(c);
        i++;
    }

    // Check if the comment state changed at the end of the line
    int changed = (row->hl_open_comment != in_comment);
    row->hl_open_comment = in_comment;

    // Propagate highlighting state changes down to next rows
    if (changed && row_idx + 1 < state->numrows) {
        highlight_update_row(state, row_idx + 1);
    }
}
