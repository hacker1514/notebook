#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "editor.h"
#include "file.h"

int main(int argc, char *argv[]) {
    EditorState state;

    // Initialize the core editor state
    if (!editor_init(&state)) {
        fprintf(stderr, "Fatal Error: Failed to initialize editor state.\n");
        return EXIT_FAILURE;
    }

    // Parse command line arguments and initialize first buffer
    if (argc > 1) {
        buffer_open(&state, argv[1]);
    } else {
        buffer_new(&state);
    }

    // Start raw mode and execute the editor main loop
    editor_run(&state);

    // Clean up allocated memory on termination
    editor_cleanup(&state);

    return EXIT_SUCCESS;
}
