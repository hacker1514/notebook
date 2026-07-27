#include <stdlib.h>
#include <string.h>
#include "syntax.h"

// C/C++ syntax definitions
static char *C_HL_extensions[] = {".c", ".h", ".cpp", ".cc", ".hpp", ".cxx", NULL};
static char *C_HL_keywords[] = {
    "switch", "if", "while", "for", "break", "continue", "return", "else",
    "struct", "union", "typedef", "static", "enum", "class", "case", "default",
    "sizeof", "volatile", "register", "extern", "const", "inline", "goto",
    "int|", "long|", "double|", "float|", "char|", "unsigned|", "signed|",
    "void|", "short|", "auto|", "bool|", "size_t|", "ssize_t|", "wchar_t|", NULL
};

// Python syntax definitions
static char *Python_HL_extensions[] = {".py", NULL};
static char *Python_HL_keywords[] = {
    "and", "as", "assert", "break", "class", "continue", "def", "del", "elif",
    "else", "except", "False", "finally", "for", "from", "global", "if", "import",
    "in", "is", "lambda", "None", "nonlocal", "not", "or", "pass", "raise",
    "return", "True", "try", "while", "with", "yield",
    "int|", "float|", "str|", "list|", "dict|", "tuple|", "set|", "bool|", NULL
};

// JavaScript/TypeScript/JSON syntax definitions
static char *JS_HL_extensions[] = {".js", ".jsx", ".ts", ".tsx", ".json", NULL};
static char *JS_HL_keywords[] = {
    "break", "case", "catch", "class", "const", "continue", "debugger", "default",
    "delete", "do", "else", "export", "extends", "finally", "for", "function",
    "if", "import", "in", "instanceof", "new", "return", "super", "switch",
    "this", "throw", "try", "typeof", "var", "void", "while", "with", "yield",
    "let", "static", "enum", "await", "async", "null", "undefined", "true", "false",
    "Number|", "String|", "Boolean|", "Array|", "Object|", "Function|", NULL
};

// HTML/XML syntax definitions
static char *HTML_HL_extensions[] = {".html", ".htm", ".xml", NULL};
static char *HTML_HL_keywords[] = {
    "doctype", "html", "head", "title", "body", "h1", "h2", "h3", "h4", "h5", "h6",
    "p", "br", "hr", "div", "span", "a", "img", "ul", "ol", "li", "table", "tr", "td",
    "th", "form", "input", "button", "textarea", "select", "option", "script", "style",
    "meta", "link", "id|", "class|", "href|", "src|", "style|", "type|", "value|", NULL
};

static editorSyntax HLDB[] = {
    {
        "C/C++",
        C_HL_extensions,
        C_HL_keywords,
        "//", "/*", "*/",
        HL_HIGHLIGHT_NUMBERS | HL_HIGHLIGHT_STRINGS
    },
    {
        "Python",
        Python_HL_extensions,
        Python_HL_keywords,
        "#", NULL, NULL,
        HL_HIGHLIGHT_NUMBERS | HL_HIGHLIGHT_STRINGS
    },
    {
        "JavaScript/TypeScript/JSON",
        JS_HL_extensions,
        JS_HL_keywords,
        "//", "/*", "*/",
        HL_HIGHLIGHT_NUMBERS | HL_HIGHLIGHT_STRINGS
    },
    {
        "HTML/XML",
        HTML_HL_extensions,
        HTML_HL_keywords,
        NULL, "<!--", "-->",
        HL_HIGHLIGHT_NUMBERS | HL_HIGHLIGHT_STRINGS
    }
};

#define HLDB_ENTRIES (sizeof(HLDB) / sizeof(HLDB[0]))

editorSyntax *syntax_get_db(int *db_size) {
    *db_size = HLDB_ENTRIES;
    return HLDB;
}

void syntax_select_by_filename(EditorState *state) {
    state->syntax = NULL;
    if (state->filename == NULL) return;

    // Locate file extension
    char *ext = strrchr(state->filename, '.');
    if (ext == NULL) return;

    for (unsigned int j = 0; j < HLDB_ENTRIES; j++) {
        editorSyntax *s = &HLDB[j];
        int i = 0;
        while (s->filematch[i]) {
            if (strcmp(ext, s->filematch[i]) == 0) {
                state->syntax = s;
                return;
            }
            i++;
        }
    }
}
