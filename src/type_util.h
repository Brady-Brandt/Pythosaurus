#pragma once 
#include "expression.h"
#include "interpret.h"


static inline bool is_integer(LiteralExpr* expr){
    return (expr->litType == LIT_INTEGER || expr->litType == LIT_BOOL);
}

static inline bool is_float(LiteralExpr* expr){
    return expr->litType == LIT_FLOAT;
}

static inline bool is_bool(LiteralExpr* expr){
    return expr->litType == LIT_BOOL;
}

static inline bool is_number(LiteralExpr* expr){
    return expr->litType == LIT_INTEGER || expr->litType == LIT_BOOL || expr->litType == LIT_FLOAT;
}

static inline bool is_string(LiteralExpr* expr){
    return expr->litType == LIT_STRING;
}

static inline bool is_none(LiteralExpr* expr){
    return expr == NULL || expr->litType == LIT_NONE;
}


//converts the expression to a float if it can 
void to_float(Interpretor *interpret, LiteralExpr* expr);

//converts the expression to an int if it can 
void to_int(Interpretor *interpret, LiteralExpr* expr);

//converts the expression to a bool if it can 
void to_bool(Interpretor *interpret, LiteralExpr* expr);

//interprets the expr as a boolean return its result 
bool as_bool(Interpretor* interpret, LiteralExpr* expr);

//returns the type of LiteralExpr 
//since LiteralExpr has a type field in the struct this function 
//is only useful for getting the type of variables
LiteralType type_of(Interpretor* interpret, LiteralExpr* expr);
