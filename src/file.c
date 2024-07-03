#include "file.h"
#include <stdio.h>
#include <stdlib.h>



File file_open(const char* file_name){
    FILE* stream = fopen(file_name, "r");
    if(stream == NULL){
        fprintf(stderr,"Failed to open: %s, ", file_name);
        perror("");
        exit(1);
    }

    File result;
    result.isOpen = true;
    result.stream = stream;
    result.name = file_name;
    result.currentLine = 1;
    return result;

}

char file_next_char(File* f){
    if(f->isOpen){
        char c = fgetc(f->stream);
        if(c == '\n'){
            f->currentLine++;
        }
        return c;
    }
    return EOF;
}

char file_peek(File* f){
    if(f->isOpen){
        fpos_t pos;
        fgetpos(f->stream, &pos);
        int c = fgetc(f->stream);
        fsetpos(f->stream, &pos);
        return c;
    }
    return EOF;
}


//skips to end of a line in a file
void file_end_line(File* f){
    char c = file_next_char(f); 
    while(c != '\n' && c != EOF){
       c = file_next_char(f);
    }
}

void file_close(File* f){
    if(f->isOpen){
        fclose(f->stream);
        f->isOpen = false;
    }
}


void file_eprint_line(File* f, unsigned int line){
    if(line < f->currentLine){
        rewind(f->stream);
        f->currentLine = 1;
    }

    char c;
    while(f->currentLine != line){
        file_end_line(f);
        c = file_next_char(f);
        if(c == EOF) return;
    }

    char buffer[128];
    int buff_size = 1;
    buffer[0] = c;
    //now we should be on the line we want to print 
    fprintf(stderr, "%s -> Line %d: ", f->name, line);
    while(f->currentLine == line){
       c = file_next_char(f); 
       buffer[buff_size++] = c;
       if(buff_size == 126){
            buffer[127] = '\0';
            fprintf(stderr, "%s", buffer);
            buff_size = 0;
       }
    }
    buffer[buff_size] = '\0';
    fprintf(stderr, "%s\n", buffer);
}
