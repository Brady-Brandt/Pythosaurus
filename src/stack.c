#include "stack.h"
#include <stdbool.h>
#include <stdio.h>


void stack_create(Stack* s){
    s->size = 0;
}

bool stack_is_empty(Stack* s){
    return s->size == 0;
}

void stack_push(Stack* s, char item){
    if(s->size == MAX_STACK_SIZE){
        fprintf(stderr, "Stack is full");
        return;
    }
    s->data[s->size++] = item;
}

char stack_pop(Stack* s){
    if(stack_is_empty(s)){
        return 0;
    } 
    return s->data[--s->size];
}

char stack_peek(Stack* s){
    if(stack_is_empty(s)){
        return 0;
    }
    return s->data[s->size - 1];
}

