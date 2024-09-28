#pragma once 
#include "interpret.h"
#include "object.h"

#define NATIVE_FUNC_COUNT 23 

//returns the absolute value of a number
ClassInstance* _abs(FuncArgs* args);

ClassInstance* all(FuncArgs* args);

ClassInstance* any(FuncArgs* args);

ClassInstance* ascii(FuncArgs* args);

ClassInstance* bin(FuncArgs* args);

ClassInstance* _bool(FuncArgs* args);

ClassInstance* bytearray(FuncArgs* args);

ClassInstance* bytes(FuncArgs* args);

ClassInstance* callable(FuncArgs* args);

ClassInstance* chr(FuncArgs* args);

ClassInstance* _float(FuncArgs* args);

ClassInstance* format(FuncArgs* args);

ClassInstance* hash(FuncArgs* args);

ClassInstance* hex(FuncArgs* args);

ClassInstance* id(FuncArgs* args);

ClassInstance* input(FuncArgs* args);

ClassInstance* _int(FuncArgs* args);

ClassInstance* len(FuncArgs* args);

ClassInstance* open(FuncArgs* args);

ClassInstance* print(FuncArgs* args);

ClassInstance* _round(FuncArgs* args);

ClassInstance* sorted(FuncArgs* args);

ClassInstance* type(FuncArgs* args);




