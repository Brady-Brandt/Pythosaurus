#include "stringtype.h"
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#define DEFAULT_STR_SIZE 10


static void string_realloc(String **s){ 
    String* old = (*s);
    old->capacity *= 2;
    String* new = malloc(sizeof(String) + old->capacity);
    new->size = old->size;
    new->capacity = old->capacity;
    new->str = (void*)new + sizeof(String);
    memcpy(new->str, old->str, old->size);
    free(old);
    *s = new;
}

String* string_create(){    
    return string_create_with_cap(DEFAULT_STR_SIZE);
}



String* string_create_with_cap(uint32_t size){
    void* str = malloc(sizeof(String) + size * sizeof(char) + 1);
    String* result = str;
    result->size = 0;
    result->capacity = size;
    result->str = str + sizeof(String);
    result->str[0] = '\0';
    return result;
}



String* string_from_str(const char* str){
    uint32_t str_len = strlen(str);
    String* result = string_create_with_cap(str_len);
    result->size = str_len;
    strcpy(result->str, str);
    return result;
}


void string_push(String** s, char c){
    String* str = *s;
    if(str->size == str->capacity - 1){
        string_realloc(s);
    }
    str = *s;
    str->str[str->size++] = c;
    str->str[str->size] = '\0';
}

void string_clear(String* s){
    s->str[0] = '\0';
    s->size = 0;
}


void string_delete(String* s){
    free(s);
}




String* string_concat(String* s1, String* s2){
    uint32_t total_size = s1->size + s2->size;
    String* result = string_create_with_cap(total_size); 
    strcpy(result->str,s1->str); 
    strcat(result->str, s2->str); 
    result->size = total_size; 
    result->str[total_size] = '\0';
    return result;
}

String* string_multiply(String* s1, uint32_t times){
    uint32_t size = s1->size * times; 
    String* result = string_create_with_cap(size); 
    result->size = size;
    uint32_t index = 0;
    for(int i = 0; i < times; i++){
        memcpy(&result->str[index], s1->str, s1->size);
        index += s1->size;
    }
    result->str[size] = '\0';
    return result;
}


