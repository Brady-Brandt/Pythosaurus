#pragma once 

#define MAX_STACK_SIZE 50

#include <stdbool.h>

typedef struct {
    int size;
    char data[MAX_STACK_SIZE];
} Stack;


void stack_create(Stack* s);

bool stack_is_empty(Stack* s);

void stack_push(Stack* s, char item);

char stack_pop(Stack* s);

char stack_peek(Stack* s);
