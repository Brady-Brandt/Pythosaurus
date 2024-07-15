#include "nativefunctions.h"
#include "allocator.h"
#include "expression.h"
#include "interpret.h"
#include "print.h"
#include "stringtype.h"
#include "type_util.h"


#include <errno.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>

#define arg(index) (LiteralExpr*)func_args_get(args, index)

LiteralExpr* print(Interpretor *interpret, FuncArgs* args){
    LiteralExpr* expr = arg(0);
    switch (expr->litType) {
        case LIT_STRING:
            printf("%s\n", expr->string->str);
            break;
        case LIT_FLOAT: 
            printf("%g\n", expr->_float);
            break;
        case LIT_INTEGER: 
            printf("%ld\n", expr->integer);
            break;
        case LIT_BOOL:
            printf("%s\n", (expr->boolean) ? "True" : "False");
            break;
        default:
            printf("None");
    }
    return None;
}


//returns the absolute value of a number
LiteralExpr* _abs(Interpretor *interpret, FuncArgs* args){ 
    LiteralExpr* expr = arg(0);
    if(!is_number(expr)){
        interpretor_throw_error(interpret, "Invalid operand for abs(): %s\n", get_literal_type(expr));
    }

    LiteralExpr result;
    if(is_integer(expr)){
        result.litType = LIT_INTEGER;
        to_int(interpret, expr);
        result.integer = labs(expr->integer);
    } else{
        result.litType = LIT_FLOAT;
        result._float = fabs(expr->_float);
    }
    allocator_add(&interpret->expressionAllocator, result, LiteralExpr);
    return allocator_peek(&interpret->expressionAllocator); 
}

LiteralExpr* all(Interpretor *interpret, FuncArgs* args);

LiteralExpr* any(Interpretor *interpret, FuncArgs* args);

LiteralExpr* ascii(Interpretor *interpret, FuncArgs* args);

LiteralExpr* bin(Interpretor *interpret, FuncArgs* args){
    LiteralExpr* expr = arg(0);
    if(!is_integer(expr)){
        interpretor_throw_error(interpret, "Invalid operand for bin(): %s\n", get_literal_type(expr));
    }
    to_int(interpret, expr);
    uint32_t bits = log2(labs(expr->integer)) + 1;
    //bits + 0b + '-' 
    uint32_t bin_size = bits + 3; 
    String* bin_str = string_create_with_cap(bin_size);
    if(expr->integer < 0){
        expr->integer = labs(expr->integer);
        string_push(&bin_str, '-');
    }
    string_push(&bin_str, '0');
    string_push(&bin_str, 'b');
    
    for(int i = 0; i < bits; i++){
        char c = ((expr->integer >> (bits - 1 - i)) & 1) ? '1' : '0';
        string_push(&bin_str, c);
    }
    LiteralExpr result;
    result.litType = LIT_STRING;
    result.string = bin_str;
    allocator_add(&interpret->expressionAllocator, result, LiteralExpr);
    return allocator_peek(&interpret->expressionAllocator); 
}

LiteralExpr* _bool(Interpretor *interpret, FuncArgs* args) { 
    LiteralExpr* expr =  arg(0);
    to_bool(interpret, expr);
    LiteralExpr result;
    result.litType = LIT_BOOL;
    result.boolean = expr->boolean;
    allocator_add(&interpret->expressionAllocator, result, LiteralExpr);
    return allocator_peek(&interpret->expressionAllocator); 
}

LiteralExpr* bytearray(Interpretor *interpret, FuncArgs* args);

LiteralExpr* bytes(Interpretor *interpret, FuncArgs* args);

LiteralExpr* callable(Interpretor *interpret, FuncArgs* args);

LiteralExpr* chr(Interpretor *interpret, FuncArgs* args);

LiteralExpr* _float(Interpretor *interpret, FuncArgs* args){
    LiteralExpr* expr = arg(0);
    LiteralExpr result;
    result.litType = LIT_FLOAT;
    if(is_string(expr)) {
        errno = 0;
        char* end;
        result._float = strtod(expr->string->str, &end);
        if(errno != 0 || *end != 0){
            interpretor_throw_error(interpret, "Failed to convert String to float: %s\n", expr->string->str);
        }
        allocator_add(&interpret->expressionAllocator, result, LiteralExpr);
        return allocator_peek(&interpret->expressionAllocator);  
    }
    to_float(interpret, expr);
    result._float = expr->_float;
    allocator_add(&interpret->expressionAllocator, result, LiteralExpr);
    return allocator_peek(&interpret->expressionAllocator);  
}

LiteralExpr* format(Interpretor *interpret, FuncArgs* args);

LiteralExpr* hash(Interpretor *interpret, FuncArgs* args);

LiteralExpr* hex(Interpretor *interpret, FuncArgs* args) {
    LiteralExpr* expr =  arg(0);
    if(!is_integer(expr)){
        interpretor_throw_error(interpret, "Invalid operand for hex(): %s\n", get_literal_type(expr));
    }
    to_int(interpret, expr);
    uint32_t digits = log(expr->integer) / log(16) + 1;
    //digits + 0x + '-' 
    uint32_t hex_size = digits + 3; 
    String* hex_str = string_create_with_cap(hex_size);
    if(expr->integer < 0){
        expr->integer = labs(expr->integer);
        string_push(&hex_str, '-');
    }
    int num_digits = sprintf(&hex_str->str[hex_str->size], "0x%lx", expr->integer);
    hex_str->size += num_digits;
    LiteralExpr result;
    result.litType = LIT_STRING;
    result.string = hex_str;
    allocator_add(&interpret->expressionAllocator, result, LiteralExpr);
    return allocator_peek(&interpret->expressionAllocator);  
}

LiteralExpr* id(Interpretor *interpret, FuncArgs* args);

LiteralExpr* input(Interpretor *interpret, FuncArgs* args){
    LiteralExpr* expr = arg(0);
    printf("%s", expr->string->str);

    String* result_string = string_create_with_cap(8);
    while(true){
        int c = fgetc(stdin);
        if(c == EOF || c == '\n') break;
        string_push(&result_string, c);
    }
     
    LiteralExpr result;
    result.litType = LIT_STRING;
    result.string = result_string;
    allocator_add(&interpret->expressionAllocator, result, LiteralExpr);
    return allocator_peek(&interpret->expressionAllocator);  
}


LiteralExpr* _int(Interpretor *interpret, FuncArgs* args){
    LiteralExpr* expr = arg(0);
    LiteralExpr result;
    result.litType = LIT_INTEGER;
    if(is_string(expr)) {
        errno = 0;
        char* end;
        result.integer = strtol(expr->string->str, &end, 10);
        if(errno != 0 || *end != 0){
            interpretor_throw_error(interpret, "Failed to convert String to int: %s\n", expr->string->str);
        }
        allocator_add(&interpret->expressionAllocator, result, LiteralExpr);
        return allocator_peek(&interpret->expressionAllocator);  
    }

    to_int(interpret, expr);
    result.integer = expr->integer;
    allocator_add(&interpret->expressionAllocator, result, LiteralExpr);
    return allocator_peek(&interpret->expressionAllocator);  
}

LiteralExpr* len(Interpretor *interpret, FuncArgs* args){ 
    LiteralExpr* expr = arg(0);
    if(is_string(expr)){
        LiteralExpr result;
        result.litType = LIT_INTEGER;
        result.integer = expr->string->size;
        allocator_add(&interpret->expressionAllocator, result, LiteralExpr);
        return allocator_peek(&interpret->expressionAllocator);  
    }  
    interpretor_throw_error(interpret, "len() not defined for: %s\n", get_literal_type(expr));
}

LiteralExpr* open(Interpretor *interpret, FuncArgs* args);

LiteralExpr* _round(Interpretor *interpret, FuncArgs* args);

LiteralExpr* sorted(Interpretor *interpret, FuncArgs* args);

LiteralExpr* type(Interpretor *interpret, FuncArgs* args);



