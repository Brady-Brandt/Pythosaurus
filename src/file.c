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
    return result;

}

char file_next_char(File* f){
    if(f->isOpen){
        return fgetc(f->stream);
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
    while(file_next_char(f) != '\n'){
        
    }
}

void file_close(File* f){
    if(f->isOpen){
        fclose(f->stream);
        f->isOpen = false;
    }
}
