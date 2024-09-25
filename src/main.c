#include <stdbool.h>
#include <stdio.h>
#include "arraylist.h"
#include "interpret.h"
#include "tokenizer.h"
#include "parser.h"
#include "arena.h"
#include "file.h"



int main(int argc, char** argv){
    if(argc < 2){
        fprintf(stderr,"Invalid number of args\n");
        return 1; 
    }

    memory_init();
  
    File file = file_open(argv[1]);
    ArrayList* tokens = tokenize_file(&file);
    Parser p;
    parser_create(&p, &file, tokens);
    ArrayList* stmts = parse_tokens(&p);
    interpt_stmts(&file, stmts);

    memory_cleanup();

    return 0;
}
