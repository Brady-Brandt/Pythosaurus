#include "type_op.h"
#include "expression.h"
#include "interpret.h"
#include "print.h"
#include "type_util.h"
#include <math.h>
#include <stdbool.h>
#include <string.h>


#define INT_OP(result, a, op, b) result->integer = a->integer op b->integer
#define FLOAT_OP(result, a, op, b) result->_float = a->_float op b->_float
#define INT_CMP(result, a, op, b) result->boolean = a->integer op b->integer
#define FLOAT_CMP(result, a, op, b) result->boolean = a->_float op b->_float


void add(Interpretor *interpret, LiteralExpr* a, LiteralExpr* b, LiteralExpr* result){
    #define OP +
    switch (a->litType) {
        case LIT_INTEGER: {
            if(!is_number(b)) goto error;
            result->litType = LIT_INTEGER;
            if(is_float(b)){
                to_float(interpret, a);
                result->litType = LIT_FLOAT;
                FLOAT_OP(result, a, OP ,b);
                break;
            }
            to_int(interpret, b);
            INT_OP(result, a, OP ,b);
            break;
        }
        case LIT_FLOAT: {
            if(!is_number(b)) goto error;
            if(is_integer(b)) to_float(interpret, b);
            result->litType = LIT_FLOAT;
            FLOAT_OP(result, a, OP,b);
            break;
        }
        case LIT_STRING: {
            if(!is_string(b)) goto error;
            result->litType = LIT_STRING;
            result->string = string_concat(a->string, b->string);
            break;
        }
        case LIT_BOOL: { 
            if(!is_number(b)) goto error;
            if(is_float(b)){
                to_float(interpret, a);
                result->litType = LIT_FLOAT;
                FLOAT_OP(result, a, OP ,b);
            } else{
                to_int(interpret, a);
                to_int(interpret, b);
                INT_OP(result, a, OP, b);
            }
            break;
        }
        default:
            error:
            interpretor_throw_error(interpret, "+ not defined between %s and %s", get_literal_type(a), get_literal_type(b)); 
    }
    #undef OP
}


void sub(Interpretor *interpret, LiteralExpr* a, LiteralExpr* b, LiteralExpr* result){
    #define OP - 
    switch (a->litType) {
        case LIT_INTEGER: {
            if(!is_number(b)) goto error;
            result->litType = LIT_INTEGER;
            if(is_float(b)){
                to_float(interpret, a);
                result->litType = LIT_FLOAT;
                FLOAT_OP(result, a, OP ,b);
                break;
            }
            to_int(interpret, b);
            INT_OP(result, a, OP,b);
            break;
        }
        case LIT_FLOAT: {
            if(!is_number(b)) goto error;
            if(is_integer(b)) to_float(interpret, b);
            result->litType = LIT_FLOAT;
            FLOAT_OP(result, a, OP ,b);
            break;
        } 
        case LIT_BOOL: { 
            if(!is_number(b)) goto error;
            if(is_float(b)){
                to_float(interpret, a);
                result->litType = LIT_FLOAT;
                FLOAT_OP(result, a, OP ,b);
            } else{
                to_int(interpret, a);
                to_int(interpret, b);
                INT_OP(result, a, OP, b);
            }
            break;
        }
        default:
            error:
            interpretor_throw_error(interpret, "- not defined between %s and %s", get_literal_type(a), get_literal_type(b)); 
    }
    #undef OP
}

void mul(Interpretor *interpret, LiteralExpr* a, LiteralExpr* b, LiteralExpr* result){
    //ensure that if there is a string it is on the left side 
    if(is_string(b)){
        LiteralExpr* temp = a;
        a = b;
        b = temp;
    }

    #define OP * 
    switch (a->litType) {
        case LIT_INTEGER: {
            if(!is_number(b)) goto error;
            result->litType = LIT_INTEGER;
            if(is_float(b)){
                to_float(interpret, a);
                result->litType = LIT_FLOAT;
                FLOAT_OP(result, a, OP ,b);
                break;
            }
            to_int(interpret, b);
            INT_OP(result, a, OP,b);
            break;
        }
        case LIT_FLOAT: {
            if(!is_number(b)) goto error;
            if(is_integer(b)) to_float(interpret, b);
            result->litType = LIT_FLOAT;
            FLOAT_OP(result, a, OP ,b);
            break;
        }
        case LIT_STRING: {
            if(!is_integer(b)) goto error;
            //ensure b is an integer
            to_int(interpret, b);
            result->litType = LIT_STRING;
            result->string = string_multiply(a->string, b->integer);
            break;
        }
        case LIT_BOOL: { 
            if(!is_number(b)) goto error;
            if(is_float(b)){
                to_float(interpret, a);
                result->litType = LIT_FLOAT;
                FLOAT_OP(result, a, OP ,b);
            } else{
                to_int(interpret, a);
                to_int(interpret, b);
                INT_OP(result, a, OP, b);
            }
            break;
        }
        default:
            error:
            interpretor_throw_error(interpret, "* not defined between %s and %s", get_literal_type(a), get_literal_type(b)); 
    }
    #undef OP
}

void divide(Interpretor *interpret, LiteralExpr* a, LiteralExpr* b, LiteralExpr* result){
    #define OP / 
    switch (a->litType) {
        //division always returns a float 
        case LIT_FLOAT:
        case LIT_BOOL:
        case LIT_INTEGER: {
            if(!is_number(b)) goto error;
            result->litType = LIT_FLOAT;
            to_float(interpret, a); 
            to_float(interpret, b); 
            FLOAT_OP(result, a, OP,b);
            break;
        } 
        default:
            error:
            interpretor_throw_error(interpret, "/ not defined between %s and %s", get_literal_type(a), get_literal_type(b)); 
    }
    #undef OP
}




void mod(Interpretor *interpret, LiteralExpr* a, LiteralExpr* b, LiteralExpr* result){
    #define OP % 
    switch (a->litType) {
        case LIT_INTEGER: {
            if(!is_number(b)) goto error;
            result->litType = LIT_INTEGER;
            if(is_float(b)){
                to_float(interpret, a);
                result->litType = LIT_FLOAT;
                result->_float = fmod(result->_float, result->_float);
                break;
            }
            to_int(interpret, b);
            INT_OP(result, a, OP,b);
            break;
        }
        case LIT_FLOAT: {
            if(!is_number(b)) goto error;
            if(is_integer(b)) to_float(interpret, b);
            result->litType = LIT_FLOAT;
            result->_float = fmod(result->_float, result->_float);
            break;
        } 
        case LIT_BOOL: { 
            if(!is_number(b)) goto error;
            if(is_float(b)){
                to_float(interpret, a);
                result->litType = LIT_FLOAT;
                result->_float = fmod(result->_float, result->_float);
            } else{
                to_int(interpret, a);
                to_int(interpret, b);
                INT_OP(result, a, OP, b);
            }
            break;
        }
        default:
            error:
            interpretor_throw_error(interpret, "% not defined between %s and %s", get_literal_type(a), get_literal_type(b)); 
    }
    #undef OP
}

void expo(Interpretor *interpret, LiteralExpr* a, LiteralExpr* b, LiteralExpr* result) {
    switch (a->litType) {
        case LIT_INTEGER: {
            if(!is_number(b)) goto error;
            result->litType = LIT_INTEGER;
            if(is_float(b)){
                to_float(interpret, a);
                result->litType = LIT_FLOAT;
                result->_float = pow(a->_float, b->_float);
                break;
            }

            to_int(interpret, b);
            result->integer = pow(a->integer, b->integer);
            break;
        }
        case LIT_FLOAT: {
            if(!is_number(b)) goto error;
            if(is_integer(b)) to_float(interpret, b);
            result->litType = LIT_FLOAT;
            result->_float = pow(a->_float, b->_float);
            break;
        } 
        case LIT_BOOL: { 
            if(!is_number(b)) goto error;
            if(is_float(b)){
                to_float(interpret, a);
                result->litType = LIT_FLOAT;
                result->_float = pow(a->_float, b->_float);
            } else{
                to_int(interpret, a);
                to_int(interpret, b);
                result->integer = pow(a->integer, b->integer);
            }
            break;
        }
        default:
            error:
            interpretor_throw_error(interpret, "** not defined between %s and %s", get_literal_type(a), get_literal_type(b)); 
    }
}

void floor_div(Interpretor *interpret, LiteralExpr* a, LiteralExpr* b, LiteralExpr* result) {
    #define OP / 
    switch (a->litType) {
        case LIT_INTEGER: {
            if(!is_number(b)) goto error;
            result->litType = LIT_INTEGER;
            if(is_float(b)){
                to_float(interpret, a);
                result->litType = LIT_FLOAT;
                FLOAT_OP(result, a, OP ,b); 
                result->_float = floor(result->_float);
                break;
            }
            to_int(interpret, b);
            INT_OP(result, a, OP,b);
            break;
        }
        case LIT_FLOAT: {
            if(!is_number(b)) goto error;
            if(is_integer(b)) to_float(interpret, b);
            result->litType = LIT_FLOAT;
            FLOAT_OP(result, a, OP ,b);
            result->_float = floor(result->_float);
            break;
        } 
        case LIT_BOOL: { 
            if(!is_number(b)) goto error;
            if(is_float(b)){
                to_float(interpret, a);
                result->litType = LIT_FLOAT;
                FLOAT_OP(result, a, OP ,b);
                result->_float = floor(result->_float);
            } else{
                to_int(interpret, a);
                to_int(interpret, b);
                INT_OP(result, a, OP, b);
            }
            break;
        }
        default:
            error:
            interpretor_throw_error(interpret, "// is not defined between %s and %s", get_literal_type(a), get_literal_type(b)); 
    }
    #undef OP
}

void and(Interpretor *interpret, LiteralExpr* a, LiteralExpr* b, LiteralExpr* result){
    result->litType = LIT_BOOL;
    result->boolean = as_bool(interpret, a) && as_bool(interpret, b);
}

void or(Interpretor *interpret, LiteralExpr* a, LiteralExpr* b, LiteralExpr* result){
    result->litType = LIT_BOOL;
    result->boolean = as_bool(interpret, a) || as_bool(interpret, b);
}

void equal(Interpretor *interpret, LiteralExpr* a, LiteralExpr* b, LiteralExpr* result){
    result->litType = LIT_BOOL;
    
    if(is_none(a) || is_none(b)){
        result->boolean = a->litType == b->litType;
        return;
    }

    if(is_number(a) != is_number(b)){
        result->boolean = false;
        return;
    }

    #define OP ==
    switch (a->litType) {
        case LIT_INTEGER: {
            if(is_float(b)){
                to_float(interpret, a);
                FLOAT_CMP(result, a, OP ,b);
                break;
            }
            to_int(interpret, b);
            FLOAT_CMP(result, a, OP ,b);
            break;
        }
        case LIT_FLOAT: {
            if(is_integer(b)) to_float(interpret, b);
            FLOAT_CMP(result, a, OP,b);
            break;
        }
        case LIT_STRING: { 
            result->boolean = strcmp(a->string->str, b->string->str) == 0;
            break;
        }
        case LIT_BOOL: { 
            if(is_float(b)){
                to_float(interpret, a);
                FLOAT_CMP(result, a, OP ,b);
            } else{
                to_int(interpret, a);
                to_int(interpret, b);
                INT_CMP(result, a, OP, b);
            }
            break;
        }
        default:
            interpretor_throw_error(interpret, "== not defined between %s and %s", get_literal_type(a), get_literal_type(b)); 
    }
    #undef OP
}

void greater_than(Interpretor *interpret, LiteralExpr* a, LiteralExpr* b, LiteralExpr* result){
    result->litType = LIT_BOOL; 
    #define OP >
    switch (a->litType) {
        case LIT_INTEGER: {
            if(!is_number(b)) goto error;
            if(is_float(b)){
                to_float(interpret, a);
                FLOAT_CMP(result, a, OP ,b);
                break;
            }
            to_int(interpret, b);
            FLOAT_CMP(result, a, OP ,b);
            break;
        }
        case LIT_FLOAT: {
            if(!is_number(b)) goto error;
            if(is_integer(b)) to_float(interpret, b);
            FLOAT_CMP(result, a, OP,b);
            break;
        }
        case LIT_STRING: { 
            if(!is_string(b)) goto error;
            result->boolean = strcmp(a->string->str, b->string->str) > 0;
            break;
        }
        case LIT_BOOL: { 
            if(!is_number(b)) goto error;
            if(is_float(b)){
                to_float(interpret, a);
                FLOAT_CMP(result, a, OP ,b);
            } else{
                to_int(interpret, a);
                to_int(interpret, b);
                INT_CMP(result, a, OP, b);
            }
            break;
        }
        default:
            error:
            interpretor_throw_error(interpret, "> not defined between %s and %s", get_literal_type(a), get_literal_type(b)); 
    }
    #undef OP
}

void greater_equal(Interpretor *interpret, LiteralExpr* a, LiteralExpr* b, LiteralExpr* result){
    result->litType = LIT_BOOL; 
    #define OP >=
    switch (a->litType) {
        case LIT_INTEGER: {
            if(!is_number(b)) goto error;
            if(is_float(b)){
                to_float(interpret, a);
                FLOAT_CMP(result, a, OP ,b);
                break;
            }
            to_int(interpret, b);
            FLOAT_CMP(result, a, OP ,b);
            break;
        }
        case LIT_FLOAT: {
            if(!is_number(b)) goto error;
            if(is_integer(b)) to_float(interpret, b);
            FLOAT_CMP(result, a, OP,b);
            break;
        }
        case LIT_STRING: { 
            if(!is_string(b)) goto error;
            result->boolean = strcmp(a->string->str, b->string->str) >= 0;
            break;
        }
        case LIT_BOOL: { 
            if(!is_number(b)) goto error;
            if(is_float(b)){
                to_float(interpret, a);
                FLOAT_CMP(result, a, OP ,b);
            } else{
                to_int(interpret, a);
                to_int(interpret, b);
                INT_CMP(result, a, OP, b);
            }
            break;
        }
        default:
            error:
            interpretor_throw_error(interpret, ">= not defined between %s and %s", get_literal_type(a), get_literal_type(b)); 
    }
    #undef OP



}

void less_than(Interpretor *interpret, LiteralExpr* a, LiteralExpr* b, LiteralExpr* result){
    result->litType = LIT_BOOL; 
    #define OP <
    switch (a->litType) {
        case LIT_INTEGER: {
            if(!is_number(b)) goto error;
            if(is_float(b)){
                to_float(interpret, a);
                FLOAT_CMP(result, a, OP ,b);
                break;
            }
            to_int(interpret, b);
            FLOAT_CMP(result, a, OP ,b);
            break;
        }
        case LIT_FLOAT: {
            if(!is_number(b)) goto error;
            if(is_integer(b)) to_float(interpret, b);
            FLOAT_CMP(result, a, OP,b);
            break;
        }
        case LIT_STRING: { 
            if(!is_string(b)) goto error;
            result->boolean = strcmp(a->string->str, b->string->str) < 0;
            break;
        }
        case LIT_BOOL: { 
            if(!is_number(b)) goto error;
            if(is_float(b)){
                to_float(interpret, a);
                FLOAT_CMP(result, a, OP ,b);
            } else{
                to_int(interpret, a);
                to_int(interpret, b);
                INT_CMP(result, a, OP, b);
            }
            break;
        }
        default:
            error:
            interpretor_throw_error(interpret, "< not defined between %s and %s", get_literal_type(a), get_literal_type(b)); 
    }
    #undef OP
}

void less_equal(Interpretor *interpret, LiteralExpr* a, LiteralExpr* b, LiteralExpr* result){
    result->litType = LIT_BOOL; 
    #define OP <=
    switch (a->litType) {
        case LIT_INTEGER: {
            if(!is_number(b)) goto error;
            if(is_float(b)){
                to_float(interpret, a);
                FLOAT_CMP(result, a, OP ,b);
                break;
            }
            to_int(interpret, b);
            FLOAT_CMP(result, a, OP ,b);
            break;
        }
        case LIT_FLOAT: {
            if(!is_number(b)) goto error;
            if(is_integer(b)) to_float(interpret, b);
            FLOAT_CMP(result, a, OP,b);
            break;
        }
        case LIT_STRING: { 
            if(!is_string(b)) goto error;
            result->boolean = strcmp(a->string->str, b->string->str) <= 0;
            break;
        }
        case LIT_BOOL: { 
            if(!is_number(b)) goto error;
            if(is_float(b)){
                to_float(interpret, a);
                FLOAT_CMP(result, a, OP ,b);
            } else{
                to_int(interpret, a);
                to_int(interpret, b);
                INT_CMP(result, a, OP, b);
            }
            break;
        }
        default:
            error:
            interpretor_throw_error(interpret, "<= not defined between %s and %s", get_literal_type(a), get_literal_type(b)); 
    }
    #undef OP
}

void bit_and(Interpretor *interpret, LiteralExpr* a, LiteralExpr* b, LiteralExpr* result){
    switch (a->litType) {
        case LIT_BOOL:
        case LIT_INTEGER: {
            if(!is_integer(b)) goto error;
            to_int(interpret, b);
            INT_OP(result, a, &, b);
            break;
        }
        default:
            error:
            interpretor_throw_error(interpret, "& is not defined between %s and %s", get_literal_type(a), get_literal_type(b));  
    }
}

void bit_or(Interpretor *interpret, LiteralExpr* a, LiteralExpr* b, LiteralExpr* result){
    switch (a->litType) {
        case LIT_BOOL:
        case LIT_INTEGER: {
            if(!is_integer(b)) goto error;
            to_int(interpret, b);
            INT_OP(result, a, |, b);
            break;
        }
        default:
            error:
            interpretor_throw_error(interpret, "| is not defined between %s and %s", get_literal_type(a), get_literal_type(b));  
    }
}

void bit_xor(Interpretor *interpret, LiteralExpr* a, LiteralExpr* b, LiteralExpr* result){
    switch (a->litType) {
        case LIT_BOOL:
        case LIT_INTEGER: {
            if(!is_integer(b)) goto error;
            to_int(interpret, b);
            INT_OP(result, a, ^, b);
            break;
        }
        default:
            error:
            interpretor_throw_error(interpret, "^ is not defined between %s and %s", get_literal_type(a), get_literal_type(b));  
    }
}

void left_shift(Interpretor *interpret, LiteralExpr* a, LiteralExpr* b, LiteralExpr* result){
    switch (a->litType) {
        case LIT_BOOL:
        case LIT_INTEGER: {
            if(!is_integer(b)) goto error;
            to_int(interpret, b);
            INT_OP(result, a, <<, b);
            break;
        }
        default:
            error:
            interpretor_throw_error(interpret, "<< is not defined between %s and %s", get_literal_type(a), get_literal_type(b));  
    }
}

void right_shift(Interpretor *interpret, LiteralExpr* a, LiteralExpr* b, LiteralExpr* result){
    switch (a->litType) {
        case LIT_BOOL:
        case LIT_INTEGER: {
            if(!is_integer(b)) goto error;
            to_int(interpret, b);
            INT_OP(result, a, >>, b);
            break;
        }
        default:
            error:
            interpretor_throw_error(interpret, ">> is not defined between %s and %s", get_literal_type(a), get_literal_type(b));  
    }
}

void uadd(Interpretor *interpret, LiteralExpr* a, LiteralExpr* result){
    if(!is_number(a)){ 
        interpretor_throw_error(interpret, "+ is not defined for %s", get_literal_type(a));  
    }
    memcpy(result, a, sizeof(LiteralExpr));
}

void usub(Interpretor *interpret, LiteralExpr* a, LiteralExpr* result){
    if(!is_number(a)){ 
        interpretor_throw_error(interpret, "- is not defined for %s", get_literal_type(a));  
    }
    
    if(is_integer(a)){
        result->litType = LIT_INTEGER;
        to_int(interpret, a);
        result->integer = -a->integer;
    } else{
        result->litType = LIT_FLOAT;
        result->_float = -a->_float;  
    }
}

void bit_not(Interpretor *interpret, LiteralExpr* a, LiteralExpr* result){
    if(!is_integer(a)){
        interpretor_throw_error(interpret, "~ is not defined for %s", get_literal_type(a));  
    }
    result->litType = LIT_INTEGER;
    if(a->litType == LIT_BOOL){
        result->integer = ~((long)a->boolean);
        return;;
    }
    result->integer = ~a->integer; 
}

void not(Interpretor *interpret, LiteralExpr* a, LiteralExpr* result){
    memcpy(result, a, sizeof(LiteralExpr));
    to_bool(interpret, result);
    result->boolean = !result->boolean;
}



