#pragma once 

#include "expression.h"
#include "statement.h"
#include "stringtype.h"
#include "stack.h"
#include "arraylist.h"
#include "hashmap.h"
#include "allocator.h"
#include "object.h"
#include <stdnoreturn.h>

struct LiteralExpr;

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


typedef ClassInstance* (*NativeMethod)(struct Interpretor*,MethodArgs*);



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



void interpretor_create_scope(Interpretor* interpret);

void interpretor_delete_scope(Interpretor* interpret);

void interpretor_assign_var(Interpretor *interpret, String* name, ClassInstance* value);

ClassInstance* interpretor_get_var(Interpretor *interpret, String* name);


void interpretor_global_var(Interpretor *interpret, String* name);

void interpretor_del_value(Interpretor *interpret, LiteralExpr* val);

void interpretor_add_class(Interpretor *interpret, Class* obj);

Class* interpretor_get_class(Interpretor *interpret, String* name);

static inline long interpretor_save_expression(Interpretor *interpret){
    return allocator_get_offset(&interpret->expressionAllocator);
}

static inline void interpretor_restore_expression(Interpretor *interpret, long offset){
    return allocator_set_offset(&interpret->expressionAllocator, offset);
}

//creates a user defined function 
void interpretor_create_function(Interpretor *interpret, FunctionStmt* func);

void interpretor_return(Interpretor* interpret, ClassInstance* value);

ClassInstance* interpretor_call_function(Interpretor* interpret, String* name, FuncArgs args);

noreturn void interpretor_throw_error(Interpretor *interpret, const char* fmt, ...);

void interpt_stmts(File* file, ArrayList* stmts);
