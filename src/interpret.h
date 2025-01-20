#pragma once 

#include "expression.h"
#include "parser.h"
#include "statement.h"
#include "stringtype.h"
#include "stack.h"
#include "hashmap.h"
#include "object.h"
#include <stdnoreturn.h>



typedef struct {
    Statement* currentStmt;
    Stack* operandStack;
    File* f;
    Stack* stackFrames; //holds all variables 
    HashMap* functions;
    HashMap* classes;
} Interpretor;



void interpretor_assign_var(String* name, ClassInstance* value);

//pushes the value of variable name onto the stack
void interpretor_push_var(String* name);

//pushes a value to the top of the operand stack
void interpretor_stack_push(ClassInstance* value);

//pops a value to the top of the operand stack
ClassInstance* interpretor_stack_pop();

ClassInstance* interpretor_stack_peek();

//swaps the first and second item on the stack
void interpretor_stack_swap_two();

//returns the type of the first arguement used for method 
//calls. The first arguement is most often self 
//we need to know the type in order to call the right method
Class* interpretor_stack_get_class_type(int argc);

void interpretor_global_var(String* name);

void interpretor_del_value(LiteralExpr* val);

void interpretor_add_class(Class* obj);

Class* interpretor_get_class(String* name);

void interpretor_set_stmt(Statement* stmt);

//creates a user defined function 
void interpretor_create_function(FunctionStmt* func);

void interpretor_return();

void interpretor_call_function(String* name, Args* args);

noreturn void interpretor_throw_error(const char* fmt, ...);

void interpt_stmts(ParserResult parser_res);
