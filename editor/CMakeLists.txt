cmake_minimum_required(VERSION 3.15)
project(notebook C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Compiler-specific warning flags
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -pedantic -O2)
endif()

add_executable(notebook
    main.c
    editor.c
    editor.h
    platform.c
    platform.h
    terminal.c
    terminal.h
)
