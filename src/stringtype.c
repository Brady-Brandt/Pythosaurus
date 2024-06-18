#include "stringtype.h"
#include <stdint.h>
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



String string_create_with_cap(uint32_t size){
    String result;
    result.size = 0;
    result.capacity = size;
    result.str = malloc(size * sizeof(char));
    result.str[0] = 0; 
    return result;
}


String string_from_str(const char* str){
    uint32_t str_len = strlen(str);
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




String string_concat(String* s1, String* s2){
    size_t total_size = s1->size + s2->size;
    String result = string_create_with_cap(total_size + 1); 
    for(int i = 0; i < s1->size; i++){
        result.str[i] = s1->str[i];
        result.size++;
    }

    int index = 0;
    for(int i = s1->size; i < total_size; i++){
        result.str[i] = s2->str[index++];
        result.size++;
    }
    result.str[total_size + 1] = '\0';
    return result;
}

String string_multiply(String* s1, uint32_t times){
    uint32_t size = s1->size * times; 
    String result = string_create_with_cap(size + 1); 
    result.size = size;
    uint32_t index = 0;
    for(int i = 0; i < times; i++){
        memcpy(&result.str[index], s1->str, s1->size);
        index += s1->size;
    }
    result.str[size + 1] = '\0';
    return result;
}


