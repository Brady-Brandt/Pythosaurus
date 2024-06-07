#include <stdio.h>
#include "tokenizer.h"

int main(int argc, char** argv){

    if(argc < 2){
        fprintf(stderr,"Invalid number of args\n");
        return 1; 
    }
    
    tokenizer_create(argv[1]);
    tokenize_file();

    return 0;
}
