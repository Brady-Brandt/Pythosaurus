#pragma once 

#include "expression.h"
#include "parser.h"
#include "statement.h"
#include "stringtype.h"
#include "stack.h"
#include "hashmap.h"
#include "allocator.h"
#include "object.h"
#include <stdnoreturn.h>


//the standard technically allows up to 255 but 
//realistically if you use more than like 5 you should consider 
//using a data structure 
#define MAX_FUNC_COUNT 10

typedef struct {
    int count;
    ClassInstance* args[MAX_FUNC_COUNT];
    //TODO: ADD kwargs and varargs
} FuncArgs;



ClassInstance* func_args_get(FuncArgs* args, int index);

void func_args_add(FuncArgs* args, ClassInstance* value);


typedef ClassInstance* (*NativeMethod)(MethodArgs*);



//this defines methods for the builtin types 
typedef struct {
    const char* name;
    NativeMethod method;
    bool self; //whether the first param is self or not
    int argCount;
} NativeMethodInfo;


typedef struct {
    Statement* currentStmt;
    Allocator expressionAllocator; //stack for computing values
    File* f;
    Stack* stackFrames; //holds all variables 
    HashMap* functions;
    HashMap* classes;

    Allocator functionAllocator;
    Allocator methodAllocator;
} Interpretor;



void interpretor_assign_var(String* name, ClassInstance* value);

ClassInstance* interpretor_get_var(String* name);


void interpretor_global_var(String* name);

void interpretor_del_value(LiteralExpr* val);

void interpretor_add_class(Class* obj);

Class* interpretor_get_class(String* name);

void interpretor_set_stmt(Statement* stmt);

ClassInstance* interpretor_alloc_expr(ClassInstance instance);

long interpretor_save_expression();

void interpretor_restore_expression(long offset);

//creates a user defined function 
void interpretor_create_function(FunctionStmt* func);

void interpretor_return(ClassInstance* value);

ClassInstance* interpretor_call_function(String* name, FuncArgs args);

noreturn void interpretor_throw_error(const char* fmt, ...);

void interpt_stmts(ParserResult parser_res);
