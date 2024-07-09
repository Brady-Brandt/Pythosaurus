#include "nativefunctions.h"
#include "expression.h"
#include "interpret.h"
#include "print.h"
#include "type_util.h"
#include <stdio.h>



LiteralExpr* print(Interpretor *interpret, FuncArgs* args){
    LiteralExpr* expr =  (LiteralExpr*)func_args_get(args, 0);
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
    LiteralExpr* expr =  (LiteralExpr*)func_args_get(args, 0);
    if(is_none(expr) || !is_number(expr)){
        interpretor_throw_error(interpret, "Invalid operand for abs(): %s\n", get_literal_type(expr));
    } 
    interpretor_throw_error(interpret, "Function not implemented\n");
}

LiteralExpr* all(Interpretor *interpret, FuncArgs* args);

LiteralExpr* any(Interpretor *interpret, FuncArgs* args);

LiteralExpr* ascii(Interpretor *interpret, FuncArgs* args);

LiteralExpr* bin(Interpretor *interpret, FuncArgs* args);

LiteralExpr* _bool(Interpretor *interpret, FuncArgs* args);

LiteralExpr* bytearray(Interpretor *interpret, FuncArgs* args);

LiteralExpr* bytes(Interpretor *interpret, FuncArgs* args);

LiteralExpr* callable(Interpretor *interpret, FuncArgs* args);

LiteralExpr* chr(Interpretor *interpret, FuncArgs* args);

LiteralExpr* _float(Interpretor *interpret, FuncArgs* args);

LiteralExpr* format(Interpretor *interpret, FuncArgs* args);

LiteralExpr* hash(Interpretor *interpret, FuncArgs* args);

LiteralExpr* hex(Interpretor *interpret, FuncArgs* args);

LiteralExpr* id(Interpretor *interpret, FuncArgs* args);

LiteralExpr* input(Interpretor *interpret, FuncArgs* args);

LiteralExpr* _int(Interpretor *interpret, FuncArgs* args);

LiteralExpr* len(Interpretor *interpret, FuncArgs* args);

LiteralExpr* open(Interpretor *interpret, FuncArgs* args);

LiteralExpr* print(Interpretor *interpret, FuncArgs* args);

LiteralExpr* _round(Interpretor *interpret, FuncArgs* args);

LiteralExpr* sorted(Interpretor *interpret, FuncArgs* args);

LiteralExpr* type(Interpretor *interpret, FuncArgs* args);



