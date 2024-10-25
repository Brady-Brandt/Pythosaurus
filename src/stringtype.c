#include "stringtype.h"
#include "arena.h"
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>



String* string_from_const_str(const char* str){
    uint32_t str_len = strlen(str);
    void* data = const_pool_alloc(sizeof(String) + str_len + 1);
    String* result = data;
    result->size = str_len;
    result->data = (char*)((char*)data + sizeof(String));
    strcpy(get_str(result), str);
    return result;
}


String* string_from_str(char* str){
    uint32_t str_len = strlen(str);
    void* data = malloc(sizeof(String) + str_len + 1);
    String* result = data;
    result->size = str_len;
    result->data = (char*)((char*)data + sizeof(String));
    strcpy(get_str(result), str);
    return result;
}


String* string_from_va(const char* fmt, ...){
    va_list list;
    va_start(list, fmt);
    char* data = scratch_buffer_vfmt(fmt, list);
    va_end(list);
    scratch_buffer_clear();
    return string_from_str(data);
}


String* string_from_va_list(const char* fmt, va_list list){
    char* data = scratch_buffer_vfmt(fmt, list);
    scratch_buffer_clear();
    return string_from_str(data);
} 


String* string_copy(String* s){
    return string_from_str(get_str(s));
}


bool string_eq(String* s1, String* s2){
    if(str_size(s1) != str_size(s2)) return false;
    return strcmp(get_str(s1), get_str(s2)) == 0;
}

void string_delete(String* s){
    //only want to free memory that was allocated with malloc 
    //the rest is allocated in our const pool 
    if(const_pool_contains_ptr(s) == false && s != NULL){
        free(s);
    }
}


char string_get_char(String* s, uint32_t index){
    if(index > str_size(s) - 1) return 0;
    return get_str(s)[index];
}


String* string_concat(String* s1, String* s2){
    uint32_t size = str_size(s1) + str_size(s2);
    void* data = malloc(sizeof(String) + size + 1);
    String* result = data;
    result->size = size;
    result->data = (char*)((char*)data + sizeof(String));
    strcpy(get_str(result),get_str(s1)); 
    strcat(get_str(result), get_str(s2)); 
    return result;
}

String* string_multiply(String* s1, uint32_t times){
    scratch_buffer_clear();
    char* str = get_str(s1);
    for(int i = 0; i < times; i++){
        for(int i = 0; i < str_size(s1); i++){
            scratch_buffer_append_char(str[i]);
        }
    }
    String* res = string_from_str(scratch_buffer_as_str());
    scratch_buffer_clear(); 
    return res;
}


