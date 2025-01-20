#include "nativefunctions.h"
#include "interpret.h"
#include "object.h"
#include "stringtype.h"
#include "arena.h"


#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


void print(Args* args){
    ClassInstance* val = interpretor_stack_peek();
    if(val == NULL){
        printf("None\n");
    } else {
        Args a;
        a.count = 1;
        call_native_method(__REPR__, &a);
        ClassInstance* str = interpretor_stack_pop();
        if(str == NotImplemented) interpretor_throw_error("Invalid operand for print(): %s", class_get_name(val));
        printf("%s\n", get_str(str->pstr));
        string_delete(str->pstr);
    }
    interpretor_stack_push(None);
}


//returns the absolute value of a number
void _abs(Args* args){ 
    ClassInstance* val = interpretor_stack_peek();
    if(val == NULL){
        goto INVALID_ARG;
    } else {
        Args a;
        a.count = 1;
        call_native_method(__ABS__, &a);
        ClassInstance* res = interpretor_stack_peek();
        if(res == NotImplemented) goto INVALID_ARG;
    } 
    INVALID_ARG:
        interpretor_throw_error("Invalid operand for abs(): %s", class_get_name(val));
}

void all(Args* args);

void any(Args* args);

void ascii(Args* args);

void bin(Args* args){
    ClassInstance* val = interpretor_stack_pop(); 
    if(!is_int_class(val)){
        //TODO: CHECK FOR INDEX METHOD
        //
        interpretor_throw_error("Invalid operand for bin()");
    }
    long* p_integer = get_primitive(val);
    long integer = *p_integer;
    uint32_t bits = log2(labs(integer)) + 1;

    scratch_buffer_clear();
    if(integer < 0){
        integer = labs(integer);
        scratch_buffer_append_char('-');
    }

    scratch_buffer_append_char('0');
    scratch_buffer_append_char('b');
    
    for(int i = 0; i < bits; i++){
        char c = ((integer >> (bits - 1 - i)) & 1) ? '1' : '0';

        scratch_buffer_append_char(c);
    }

    String* bin_num = string_from_str(scratch_buffer_as_str());

    scratch_buffer_clear();
    new_str(bin_num);
}

void _bool(Args* args) { 
    ClassInstance* val =  interpretor_stack_peek();
    if(val == NULL){
        goto INVALID_ARG;
    } else {
        Args a;
        a.count = 1;
        call_native_method(__BOOL__, &a);
        ClassInstance* res = interpretor_stack_peek();
        if(res == NotImplemented) goto INVALID_ARG;
    } 
    INVALID_ARG:
        interpretor_throw_error("Invalid operand for bool()");
}

void bytearray(Args* args);

void bytes(Args* args);

void callable(Args* args);

void chr(Args* args);

void _float(Args* args){
    ClassInstance* val =  interpretor_stack_peek();
    if(val == NULL){
        goto INVALID_ARG;
    } else {
        Args a;
        a.count = 1;
        call_native_method(__FLOAT__, &a);
        ClassInstance* res = interpretor_stack_peek();
        if(res == NotImplemented) goto INVALID_ARG;
    } 
    INVALID_ARG:     
        interpretor_throw_error("Invalid operand for float()");
}

void format(Args* args);

void hash(Args* args);

void hex(Args* args) {
    ClassInstance* val =  interpretor_stack_pop();
    if(!is_int_class(val)){
        //TODO: CHECK FOR INDEX METHOD
        interpretor_throw_error("Invalid operand for hex(): %s\n");
    }
    long* p_integer = get_primitive(val);
    long integer = *p_integer;
    String* res = string_from_va("0x%lx", integer);
    new_str(res);  
}

void id(Args* args);

void input(Args* args){
    ClassInstance* val = interpretor_stack_pop();
    if(val == NULL || val->classType != &PRIM_TYPE_STR){ 
        interpretor_throw_error("Invalid operand for input()");
    }

    printf("%s", get_str(val->pstr));
    scratch_buffer_clear();

    while(true){
        int c = fgetc(stdin);
        if(c == EOF || c == '\n') break;
        scratch_buffer_append_char(c);
    }
    String* res = string_from_str(scratch_buffer_as_str());
    scratch_buffer_clear();
    new_str(res);
}


void _int(Args* args){
    ClassInstance* val =  interpretor_stack_peek();
    if(val == NULL){
        goto INVALID_ARG;
    } else {
        Args a;
        a.count = 1;
        call_native_method(__INT__, &a);
        ClassInstance* res = interpretor_stack_peek();
        if(res == NotImplemented) goto INVALID_ARG;
    } 
    INVALID_ARG:     
        interpretor_throw_error("Invalid operand for int()");
}

void len(Args* args){ 
    ClassInstance* val =  interpretor_stack_peek();
    if(val == NULL){
        goto INVALID_ARG;
    } else {
        Args a;
        a.count = 1;
        call_native_method(__LEN__, &a);
        ClassInstance* res = interpretor_stack_peek();
        if(res == NotImplemented) goto INVALID_ARG;
    } 
    INVALID_ARG:     
        interpretor_throw_error("Invalid operand for len()");
}

void open(Args* args);

void _round(Args* args);

void sorted(Args* args);

void type(Args* args);



