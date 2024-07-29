#include "nativefunctions.h"
#include "interpret.h"
#include "object.h"
#include "stringtype.h"


#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>

#define arg(index) func_args_get(args, index)

ClassInstance* print(Interpretor *interpret, FuncArgs* args){
    ClassInstance* val = arg(0);
    if(val == NULL){
        printf("None\n");
    } else {
        MethodArgs a;
        a.args[0] = val;
        a.count = 1;
        a.self = true;
        ClassInstance* str = call_native_method((struct Interpretor*)interpret, val, __REPR__, &a);
        if(str == NotImplemented) interpretor_throw_error(interpret, "Invalid operand for print(): %s", class_get_name(val));
        printf("%s\n", str->pstr->str);
        string_delete(str->pstr);
    }
    return None;
}


//returns the absolute value of a number
ClassInstance* _abs(Interpretor *interpret, FuncArgs* args){ 
    ClassInstance* val = arg(0);
    if(val == NULL){
        goto INVALID_ARG;
    } else {
        MethodArgs a;
        a.args[0] = val;
        a.count = 1;
        a.self = true;
        ClassInstance* res = call_native_method((struct Interpretor*)interpret, val, __ABS__, &a);
        if(res == NotImplemented) goto INVALID_ARG;
        return res;
    } 
    INVALID_ARG:
        interpretor_throw_error(interpret, "Invalid operand for abs(): %s", class_get_name(val));
}

ClassInstance* all(Interpretor *interpret, FuncArgs* args);

ClassInstance* any(Interpretor *interpret, FuncArgs* args);

ClassInstance* ascii(Interpretor *interpret, FuncArgs* args);

ClassInstance* bin(Interpretor *interpret, FuncArgs* args){
    ClassInstance* val = arg(0); 
    if(!is_int_class(val)){
        //TODO: CHECK FOR INDEX METHOD
        //
        interpretor_throw_error(interpret, "Invalid operand for bin()");
    }
    long* p_integer = get_primitive(val);
    long integer = *p_integer;
    uint32_t bits = log2(labs(integer)) + 1;
    //bits + 0b + '-' 
    uint32_t bin_size = bits + 3; 
    String* bin_str = string_create_with_cap(bin_size);
    if(integer < 0){
        integer = labs(integer);
        string_push(&bin_str, '-');
    }
    string_push(&bin_str, '0');
    string_push(&bin_str, 'b');
    
    for(int i = 0; i < bits; i++){
        char c = ((integer >> (bits - 1 - i)) & 1) ? '1' : '0';
        string_push(&bin_str, c);
    }
    return new_str((struct Interpretor*)interpret, bin_str);
}

ClassInstance* _bool(Interpretor *interpret, FuncArgs* args) { 
    ClassInstance* val =  arg(0);
    if(val == NULL){
        goto INVALID_ARG;
    } else {
        MethodArgs a;
        a.args[0] = val;
        a.count = 1;
        a.self = true;
        ClassInstance* res = call_native_method((struct Interpretor*)interpret, val, __BOOL__, &a);
        if(res == NotImplemented) goto INVALID_ARG;
        return res;
    } 
    INVALID_ARG:
        interpretor_throw_error(interpret, "Invalid operand for bool()");
}

ClassInstance* bytearray(Interpretor *interpret, FuncArgs* args);

ClassInstance* bytes(Interpretor *interpret, FuncArgs* args);

ClassInstance* callable(Interpretor *interpret, FuncArgs* args);

ClassInstance* chr(Interpretor *interpret, FuncArgs* args);

ClassInstance* _float(Interpretor *interpret, FuncArgs* args){
    ClassInstance* val =  arg(0);
    if(val == NULL){
        goto INVALID_ARG;
    } else {
        MethodArgs a;
        a.args[0] = val;
        a.count = 1;
        a.self = true;
        ClassInstance* res = call_native_method((struct Interpretor*)interpret, val, __FLOAT__, &a);
        if(res == NotImplemented) goto INVALID_ARG;
        return res;
    } 
    INVALID_ARG:     
        interpretor_throw_error(interpret, "Invalid operand for float()");
}

ClassInstance* format(Interpretor *interpret, FuncArgs* args);

ClassInstance* hash(Interpretor *interpret, FuncArgs* args);

ClassInstance* hex(Interpretor *interpret, FuncArgs* args) {
    ClassInstance* val =  arg(0);
    if(!is_int_class(val)){
        //TODO: CHECK FOR INDEX METHOD
        interpretor_throw_error(interpret, "Invalid operand for hex(): %s\n");
    }
    long* p_integer = get_primitive(val);
    long integer = *p_integer;

    uint32_t digits = log(integer) / log(16) + 1;
    //digits + 0x + '-' 
    uint32_t hex_size = digits + 3; 
    String* hex_str = string_create_with_cap(hex_size);
    if(integer < 0){
        integer = labs(integer);
        string_push(&hex_str, '-');
    }
    int num_digits = sprintf(&hex_str->str[hex_str->size], "0x%lx",integer);
    hex_str->size += num_digits;
    return new_str((struct Interpretor*)interpret, hex_str);  
}

ClassInstance* id(Interpretor *interpret, FuncArgs* args);

ClassInstance* input(Interpretor *interpret, FuncArgs* args){
    ClassInstance* val = arg(0);
    if(val == NULL || val->classType != &PRIM_TYPE_STR){ 
        interpretor_throw_error(interpret, "Invalid operand for input()");
    }

    printf("%s", val->pstr->str);

    String* result_string = string_create_with_cap(8);
    while(true){
        int c = fgetc(stdin);
        if(c == EOF || c == '\n') break;
        string_push(&result_string, c);
    }
    return new_str((struct Interpretor*)interpret, result_string);
}


ClassInstance* _int(Interpretor *interpret, FuncArgs* args){
    ClassInstance* val =  arg(0);
    if(val == NULL){
        goto INVALID_ARG;
    } else {
        MethodArgs a;
        a.args[0] = val;
        a.count = 1;
        a.self = true;
        ClassInstance* res = call_native_method((struct Interpretor*)interpret, val, __INT__, &a);
        if(res == NotImplemented) goto INVALID_ARG;
        return res;
    } 
    INVALID_ARG:     
        interpretor_throw_error(interpret, "Invalid operand for int()");
}

ClassInstance* len(Interpretor *interpret, FuncArgs* args){ 
    ClassInstance* val =  arg(0);
    if(val == NULL){
        goto INVALID_ARG;
    } else {
        MethodArgs a;
        a.args[0] = val;
        a.count = 1;
        a.self = true;
        ClassInstance* res = call_native_method((struct Interpretor*)interpret, val, __LEN__, &a);
        if(res == NotImplemented) goto INVALID_ARG;
        return res;
    } 
    INVALID_ARG:     
        interpretor_throw_error(interpret, "Invalid operand for len()");
}

ClassInstance* open(Interpretor *interpret, FuncArgs* args);

ClassInstance* _round(Interpretor *interpret, FuncArgs* args);

ClassInstance* sorted(Interpretor *interpret, FuncArgs* args);

ClassInstance* type(Interpretor *interpret, FuncArgs* args);



