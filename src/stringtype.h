#pragma once
#include <stdint.h>

typedef struct{
    char* str;
    uint32_t size;
    uint32_t capacity;
} String;


String* string_create();

String* string_create_with_cap(uint32_t size);

String* string_from_str(const char* str);

void string_push(String** s, char c);

void string_clear(String* s);

void string_delete(String* s);

void string_append_str(String** s, const char* str, uint32_t str_size);

char string_pop(String* s);

char string_get_char(String* s, uint32_t index);

String* string_concat(String* s1, String* s2);

String* string_multiply(String* s1, uint32_t times);




