#include "nativefunctions.h"
#include "interpret.h"
#include "object.h"
#include "stringtype.h"
#include "arena.h"


#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>

#define arg(index) func_args_get(args, index)

ClassInstance* print(FuncArgs* args){
    ClassInstance* val = arg(0);
    if(val == NULL){
        printf("None\n");
    } else {
        MethodArgs a;
        a.args[0] = val;
        a.count = 1;
        a.self = true;
        ClassInstance* str = call_native_method(val, __REPR__, &a);
        if(str == NotImplemented) interpretor_throw_error("Invalid operand for print(): %s", class_get_name(val));
        printf("%s\n", get_str(str->pstr));
        string_delete(str->pstr);
    }
    return None;
}


//returns the absolute value of a number
ClassInstance* _abs(FuncArgs* args){ 
    ClassInstance* val = arg(0);
    if(val == NULL){
        goto INVALID_ARG;
    } else {
        MethodArgs a;
        a.args[0] = val;
        a.count = 1;
        a.self = true;
        ClassInstance* res = call_native_method(val, __ABS__, &a);
        if(res == NotImplemented) goto INVALID_ARG;
        return res;
    } 
    INVALID_ARG:
        interpretor_throw_error("Invalid operand for abs(): %s", class_get_name(val));
}

ClassInstance* all(FuncArgs* args);

ClassInstance* any(FuncArgs* args);

ClassInstance* ascii(FuncArgs* args);

ClassInstance* bin(FuncArgs* args){
    ClassInstance* val = arg(0); 
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
    return new_str(bin_num);
}

ClassInstance* _bool(FuncArgs* args) { 
    ClassInstance* val =  arg(0);
    if(val == NULL){
        goto INVALID_ARG;
    } else {
        MethodArgs a;
        a.args[0] = val;
        a.count = 1;
        a.self = true;
        ClassInstance* res = call_native_method(val, __BOOL__, &a);
        if(res == NotImplemented) goto INVALID_ARG;
        return res;
    } 
    INVALID_ARG:
        interpretor_throw_error("Invalid operand for bool()");
}

ClassInstance* bytearray(FuncArgs* args);

ClassInstance* bytes(FuncArgs* args);

ClassInstance* callable(FuncArgs* args);

ClassInstance* chr(FuncArgs* args);

ClassInstance* _float(FuncArgs* args){
    ClassInstance* val =  arg(0);
    if(val == NULL){
        goto INVALID_ARG;
    } else {
        MethodArgs a;
        a.args[0] = val;
        a.count = 1;
        a.self = true;
        ClassInstance* res = call_native_method(val, __FLOAT__, &a);
        if(res == NotImplemented) goto INVALID_ARG;
        return res;
    } 
    INVALID_ARG:     
        interpretor_throw_error("Invalid operand for float()");
}

ClassInstance* format(FuncArgs* args);

ClassInstance* hash(FuncArgs* args);

ClassInstance* hex(FuncArgs* args) {
    ClassInstance* val =  arg(0);
    if(!is_int_class(val)){
        //TODO: CHECK FOR INDEX METHOD
        interpretor_throw_error("Invalid operand for hex(): %s\n");
    }
    long* p_integer = get_primitive(val);
    long integer = *p_integer;
    String* res = string_from_va("0x%lx", integer);
    return new_str(res);  
}

ClassInstance* id(FuncArgs* args);

ClassInstance* input(FuncArgs* args){
    ClassInstance* val = arg(0);
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
    return new_str(res);
}


ClassInstance* _int(FuncArgs* args){
    ClassInstance* val =  arg(0);
    if(val == NULL){
        goto INVALID_ARG;
    } else {
        MethodArgs a;
        a.args[0] = val;
        a.count = 1;
        a.self = true;
        ClassInstance* res = call_native_method(val, __INT__, &a);
        if(res == NotImplemented) goto INVALID_ARG;
        return res;
    } 
    INVALID_ARG:     
        interpretor_throw_error("Invalid operand for int()");
}

ClassInstance* len(FuncArgs* args){ 
    ClassInstance* val =  arg(0);
    if(val == NULL){
        goto INVALID_ARG;
    } else {
        MethodArgs a;
        a.args[0] = val;
        a.count = 1;
        a.self = true;
        ClassInstance* res = call_native_method(val, __LEN__, &a);
        if(res == NotImplemented) goto INVALID_ARG;
        return res;
    } 
    INVALID_ARG:     
        interpretor_throw_error("Invalid operand for len()");
}

ClassInstance* open(FuncArgs* args);

ClassInstance* _round(FuncArgs* args);

ClassInstance* sorted(FuncArgs* args);

ClassInstance* type(FuncArgs* args);



