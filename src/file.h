#pragma once

#include <stdbool.h>
#include <stdio.h>

typedef struct{
    FILE* stream;
    bool isOpen;
    unsigned int currentLine;
    const char* name;
} File;


File file_open(const char* file_name);

void file_end_line(File* f);

char file_next_char(File* f);

char file_peek(File* f);

void file_close(File* f);

void file_eprint_line(File* f, unsigned int line);
