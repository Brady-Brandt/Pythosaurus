#pragma once 

#include "expression.h"
#include "statement.h"
#include "stringtype.h"
#include "stack.h"
#include "arraylist.h"
#include "hashmap.h"
#include "allocator.h"
#include <stdnoreturn.h>

struct LiteralExpr;

//the standard technically allows up to 255 but 
//realistically if you use more than like 5 you should consider 
//using a data structure 
#define MAX_FUNC_COUNT 10

typedef struct {
    int count;
    struct LiteralExpr* args[MAX_FUNC_COUNT];
    //TODO: ADD kwargs and varargs
} FuncArgs;



struct LiteralExpr* func_args_get(FuncArgs* args, int index);

void func_args_add(FuncArgs* args, struct LiteralExpr* value);


typedef struct {
    Statement* currentStmt;
    Allocator expressionAllocator; //stack for computing values
    File* f;
    Stack stackFrames; //holds all variables 
    HashMap* functions;
} Interpretor;



void interpretor_create_scope(Interpretor* interpret);

void interpretor_delete_scope(Interpretor* interpret);

void interpretor_assign_var(Interpretor *interpret, String* name, struct LiteralExpr* value);

void interpretor_get_var(Interpretor *interpret, String* name, struct LiteralExpr* result);


static inline long interpretor_save_expression(Interpretor *interpret){
    return allocator_get_offset(&interpret->expressionAllocator);
}

static inline void interpretor_restore_expression(Interpretor *interpret, long offset){
    return allocator_set_offset(&interpret->expressionAllocator, offset);
}

//creates a user defined function 
void interpretor_create_function(Interpretor *interpret, FunctionStmt* func);

void interpretor_return(Interpretor* interpret, LiteralExpr* value);

struct LiteralExpr* interpretor_call_function(Interpretor* interpret, String* name, FuncArgs args);

noreturn void interpretor_throw_error(Interpretor *interpret, const char* fmt, ...);

void interpt_stmts(File* file, ArrayList stmts);
