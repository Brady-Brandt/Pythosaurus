#pragma once 
#include "expression.h"
#include "interpret.h"


//returns the absolute value of a number
LiteralExpr* _abs(Interpretor *interpret, FuncArgs* args);

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




