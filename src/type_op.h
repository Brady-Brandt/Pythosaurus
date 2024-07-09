#pragma once 
#include "interpret.h"
#include "expression.h"

//these function provide operation implementations for the primitive types 
//FLOAT, INTEGER, STRING, etc


void add(Interpretor *interpret, LiteralExpr* a, LiteralExpr* b, LiteralExpr* result);

void sub(Interpretor *interpret, LiteralExpr* a, LiteralExpr* b, LiteralExpr* result);

void mul(Interpretor *interpret, LiteralExpr* a, LiteralExpr* b, LiteralExpr* result);

void divide(Interpretor *interpret, LiteralExpr* a, LiteralExpr* b, LiteralExpr* result);

void mod(Interpretor *interpret, LiteralExpr* a, LiteralExpr* b, LiteralExpr* result);

void expo(Interpretor *interpret, LiteralExpr* a, LiteralExpr* b, LiteralExpr* result);

void floor_div(Interpretor *interpret, LiteralExpr* a, LiteralExpr* b, LiteralExpr* result);

void and(Interpretor *interpret, LiteralExpr* a, LiteralExpr* b, LiteralExpr* result);

void or(Interpretor *interpret, LiteralExpr* a, LiteralExpr* b, LiteralExpr* result);

void equal(Interpretor *interpret, LiteralExpr* a, LiteralExpr* b, LiteralExpr* result);

void less_than(Interpretor *interpret, LiteralExpr* a, LiteralExpr* b, LiteralExpr* result);

void less_equal(Interpretor *interpret, LiteralExpr* a, LiteralExpr* b, LiteralExpr* result);

void greater_than(Interpretor *interpret, LiteralExpr* a, LiteralExpr* b, LiteralExpr* result);

void greater_equal(Interpretor *interpret, LiteralExpr* a, LiteralExpr* b, LiteralExpr* result);

void bit_and(Interpretor *interpret, LiteralExpr* a, LiteralExpr* b, LiteralExpr* result);


void bit_or(Interpretor *interpret, LiteralExpr* a, LiteralExpr* b, LiteralExpr* result);

void bit_xor(Interpretor *interpret, LiteralExpr* a, LiteralExpr* b, LiteralExpr* result);

void left_shift(Interpretor *interpret, LiteralExpr* a, LiteralExpr* b, LiteralExpr* result);

void right_shift(Interpretor *interpret, LiteralExpr* a, LiteralExpr* b, LiteralExpr* result);

void uadd(Interpretor *interpret, LiteralExpr* a,LiteralExpr* result);

void usub(Interpretor *interpret, LiteralExpr* a, LiteralExpr* result);

void bit_not(Interpretor *interpret, LiteralExpr* a, LiteralExpr* result);

void not(Interpretor *interpret, LiteralExpr* a, LiteralExpr* result);



