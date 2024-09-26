#pragma once

#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>


//strings are going to be fixed sized heap allocated objects 
//first 32 bits are the size of the string 
typedef struct {
    uint32_t size;
    char* data;
} String;


#define str_size(string) ((string == 0) ? 0 : string->size)
#define get_str(string) (string->data)


//the string gets allocated to the constant arena 
String* string_from_const_str(const char* str);
 
String* string_from_str(char* str);

String* string_from_va(const char* fmt, ...);

String* string_from_va_list(const char* fmt, va_list list); 

String* string_copy(String* s);

bool string_eq(String* s1, String* s2);

void string_delete(String* s);

char string_get_char(String* s, uint32_t index);

String* string_concat(String* s1, String* s2);

String* string_multiply(String* s1, uint32_t times);
