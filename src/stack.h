#pragma once 

#include "arraylist.h"
#include <stdbool.h>

typedef ArrayList Stack;


#define stack_create(s, type, size) array_list_create_cap(s, type, size)


#define stack_is_empty(s) (s.size == 0) 

#define stack_push(s, type, item) \
    do { \
        if(s.size != s.capacity){ \
            array_list_append(s, type, item); \
        } \
    } while(0) \


#define stack_pop(s, type) array_list_get(s, type, --s.size)

#define stack_peek(s, type) array_list_get(s, type, s.size - 1)

#define stack_delete(s) array_list_delete(s)
