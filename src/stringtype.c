#include "stringtype.h"
#include <string.h>
#include <stdlib.h>

#define DEFAULT_STR_SIZE 16


static void string_realloc(String *s){
    s->capacity *= 2;
    s->str = realloc(s->str, s->capacity);
}

String string_create(){
    String result;
    result.size = 0;
    result.capacity = DEFAULT_STR_SIZE;
    result.str = malloc(DEFAULT_STR_SIZE * sizeof(char));
    result.str[0] = 0;

    return result;
}

String string_from_str(const char* str){
    int str_len = strlen(str);
    String result;
    result.size = str_len;
    //account for null terminator
    result.capacity = str_len + 1;
    result.str = malloc(result.capacity * sizeof(char));

    memcpy(result.str, str, result.capacity);

    return result;
}


void string_push(String* s, char c){
    if(s->size == s->capacity - 1){
        string_realloc(s);
    }
    s->str[s->size++] = c;
    s->str[s->size] = '\0';
}

void string_clear(String* s){
    s->str[0] = '\0';
    s->size = 0;
}


void string_delete(String* s){
    s->size = 0;
    s->capacity = 0;
    free(s->str);
}



