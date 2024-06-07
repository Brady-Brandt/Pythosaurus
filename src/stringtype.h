#pragma once


typedef struct{
    char* str;
    int size;
    int capacity;
} String;

#define DEFAULT_STR (String){NULL, 0, 0}


String string_create();

String string_from_str(const char* str);


void string_push(String* s, char c);


void string_clear(String* s);


void string_delete(String* s);




