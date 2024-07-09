#include "type_util.h"
#include "expression.h"



void to_float(Interpretor *interpret, LiteralExpr* expr){
    expr->litType = LIT_FLOAT;
    switch (expr->litType) {
        case LIT_FLOAT: 
            return; 
        case LIT_INTEGER:
            expr->_float = (double)expr->integer;
            return;
        case LIT_BOOL:
            expr->_float = (double)expr->boolean;
            return;
        default:
            interpretor_throw_error(interpret, "ConversionError: Failed to convert to float\n");
    }
}


bool as_bool(Interpretor* interpret, LiteralExpr* expr){
    switch (expr->litType) {
        case LIT_INTEGER: 
            return expr->integer; 
        case LIT_BOOL:
            return expr->boolean;
        case LIT_FLOAT:
            return expr->_float;
        case LIT_STRING:
            return expr->string != NULL && expr->string->size != 0;
        case LIT_NONE:
            return false;
        case LIT_IDENTIFIER: {
            LiteralExpr var;
            interpretor_get_var(interpret, expr->identifier, (struct LiteralExpr*)&var);
            return as_bool(interpret, &var);
        }
        default:
            interpretor_throw_error(interpret, "ConversionError: Failed to convert to float\n");
    }
}

void to_int(Interpretor *interpret, LiteralExpr* expr){
    switch (expr->litType) {
        case LIT_INTEGER: 
            return; 
        case LIT_BOOL:
            expr->litType = LIT_INTEGER;
            expr->integer = (long)expr->boolean;
            return;
        case LIT_FLOAT:
            expr->litType = LIT_INTEGER;
            expr->integer = (long)expr->_float;
            return;
        default:
            interpretor_throw_error(interpret, "ConversionError: Failed to convert to int\n");
    }
}


void to_bool(Interpretor *interpret, LiteralExpr* expr){
    switch (expr->litType) {
        case LIT_BOOL: 
            return; 
        case LIT_INTEGER:
            expr->litType = LIT_BOOL;
            expr->boolean = expr->integer;
            return;
        case LIT_FLOAT:
            expr->litType = LIT_BOOL;
            expr->boolean = expr->_float;
            return;
        case LIT_STRING:
            expr->litType = LIT_BOOL;
            expr->boolean = expr->string != NULL && expr->string->size != 0;
            return;
        case LIT_NONE:
            expr->litType = LIT_BOOL;
            expr->boolean = false;
            return;
        default:
            interpretor_throw_error(interpret, "ConversionError: Failed to convert to bool\n");
    }
}


LiteralType type_of(Interpretor* interpret, LiteralExpr* expr){
    if(expr == NULL) return LIT_NONE;

    if(expr->litType != LIT_IDENTIFIER) return expr->litType;

    LiteralExpr var;
    interpretor_get_var(interpret, expr->identifier, (struct LiteralExpr*)&var);
    return var.litType;
}
