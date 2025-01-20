#pragma once 

#include <stdint.h>

typedef struct {
    uint32_t size;
    uint32_t cap;
    void* data;
} Stack;


#define stack_create(stack, type, capacity) \
do { \
    stack = const_pool_alloc(sizeof(Stack) + sizeof(type) * capacity); \
    stack->size = 0; \
    stack->cap = capacity; \
    stack->data = ((char*)stack + sizeof(Stack)); \
} while(0) 


#define stack_is_empty(s) (s->size == 0) 

#define stack_is_full(s) (s->size == s->cap)

#define stack_size(s) (s->size)

#define stack_get(s, type, index) ((type*)(s->data))[index]

#define stack_push(s, type, item) \
    do { \
        if(s->size != s->cap){ \
            ((type*)(s->data))[s->size++] = item; \
        } \
    } while(0) \


#define stack_pop(s, type) ((type*)(s->data))[--s->size]

#define stack_peek(s, type) ((type*)(s->data))[s->size - 1]


