#include "interpret.h"
#include "arraylist.h"
#include "expression.h"
#include "file.h"
#include "hashmap.h"
#include "parser.h"
#include "print.h"
#include "stack.h"
#include "statement.h"
#include "stringtype.h"
#include "evaluate.h"
#include "nativefunctions.h"
#include "object.h"
#include "arena.h"

#include <setjmp.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>



#define MAX_SCOPE_DEPTH 255 


static Interpretor interpret = {0};



void interpretor_set_stmt(Statement* stmt){
    interpret.currentStmt = stmt;
}

noreturn void interpretor_throw_error(const char* fmt, ...) {
    fprintf(stderr, "Error: \n");
    if(interpret.currentStmt != NULL){
        file_eprint_line(interpret.f, interpret.currentStmt->line);
    }
    va_list list;
    va_start(list, fmt);
    vfprintf(stderr, fmt, list);
    va_end(list);
    exit(EXIT_FAILURE);
}



typedef struct {
    jmp_buf start; 
    HashMap* variables;
    uint64_t globalOffset;
} Scope;


void interpretor_create_scope(uint32_t size){
    if(size == 0){
        size = 2;
    }
    if(interpret.stackFrames->size == MAX_SCOPE_DEPTH - 1){
        interpretor_throw_error("Maximum scope depth of %d exceeded\n", MAX_SCOPE_DEPTH);
    }
    uint64_t global_off = arena_get_offset();

    Scope* result = arena_alloc(sizeof(Scope));
    memset(result, 0, sizeof(Scope));
    if(result == NULL) interpretor_throw_error("Out of memory error\n");
    result->variables = hash_map_create(size, delete_class_instance);
    result->globalOffset = global_off;
    stack_push(interpret.stackFrames, Scope*, result);
    
}

void interpretor_delete_scope(){
    if(stack_is_empty(interpret.stackFrames)) return;
    Scope* s = stack_pop(interpret.stackFrames, Scope*);
    hash_map_delete(s->variables);
    arena_set_offset(s->globalOffset);    
}




//pushes a value to the top of the operand stack
void interpretor_stack_push(ClassInstance* value){
    if(stack_is_full(interpret.operandStack)){
        interpretor_throw_error("Stack overflow\n");
    }
    stack_push(interpret.operandStack, ClassInstance, *value); 
}


ClassInstance* interpretor_stack_peek(){
    if(stack_is_empty(interpret.operandStack)) return None;
    return &stack_peek(interpret.operandStack, ClassInstance);
}

//pops a value to the top of the operand stack
ClassInstance* interpretor_stack_pop(){
    if(stack_is_empty(interpret.operandStack)) return None;

    return &stack_pop(interpret.operandStack, ClassInstance);
}

//swaps the first and second item on the stack
void interpretor_stack_swap_two(){
    if(stack_is_empty(interpret.operandStack)) return;
    ClassInstance a = stack_pop(interpret.operandStack, ClassInstance);
    if(stack_is_empty(interpret.operandStack)){
        interpretor_throw_error("Invalid stack operation\n");
        return;
    } 
    ClassInstance b = stack_pop(interpret.operandStack, ClassInstance);
    stack_push(interpret.operandStack, ClassInstance, b);
    stack_push(interpret.operandStack, ClassInstance, a); 
}

//returns the type of the first arguement used for method 
//calls. The first arguement is most often self 
//we need to know the type in order to call the right method
Class* interpretor_stack_get_class_type(int argc){
    if(argc <= 0){
        return &PRIM_TYPE_NOT_IMPLEMENTED;
    }

    int size = stack_size(interpret.operandStack);
    if(size < argc){
        interpretor_throw_error("Invalid parameters\n");
    }
    ClassInstance* temp = &stack_get(interpret.operandStack, ClassInstance, size - argc); 
    return temp->classType;
}



void interpretor_assign_var(String* name, ClassInstance* value){
 
    //check if the variable exists first
    Scope* local_scope = stack_peek(interpret.stackFrames, Scope*);
    void* existing_value = hash_map_get_value(local_scope->variables, name);
    if(existing_value != NULL){
        delete_class_instance(existing_value);
        memcpy(existing_value, value, sizeof(ClassInstance));
        return;
    }
    //if the variable doesn't exist we alloc a new spot for it 
    ClassInstance* result = arena_alloc(sizeof(ClassInstance)); 
    memcpy(result, value, sizeof(ClassInstance));
    hash_map_add_entry(local_scope->variables, name, result);
}



void interpretor_push_var(String* name){ 
    Scope* local_scope = stack_peek(interpret.stackFrames, Scope*);
    ClassInstance* value = hash_map_get_value(local_scope->variables, name);
    if(value != NULL && value != NotImplemented){
        interpretor_stack_push(value);
        return;
    } 
    interpretor_throw_error("Variable %s not defined\n", get_str(name));
}


void interpretor_global_var(String* name){

    interpretor_throw_error("Not Implemented yet");
    Scope* local_scope = stack_peek(interpret.stackFrames, Scope*);
    ClassInstance* value = hash_map_get_value(local_scope->variables, name);
    if(value != NULL){
        interpretor_throw_error("%s assigned before global declaration", name);
    }

    Scope* global_scope = array_list_get(interpret.stackFrames, Scope*, 0);
    value = hash_map_get_value(global_scope->variables, name);
 
}

void interpretor_del_value(LiteralExpr* val){
    if(val->litType != LIT_IDENTIFIER){
        interpretor_throw_error("Cannot delete: %s", get_literal_type(val));
    }
    Scope* local_scope = stack_peek(interpret.stackFrames, Scope*);
    ClassInstance* value = hash_map_delete_entry(local_scope->variables, val->identifier); 
    if(value == NULL) interpretor_throw_error("%s not defined", get_str(val->identifier));
}


static void delete_functions(void* variable){}

void interpretor_create_function(FunctionStmt* func) {
   Function* funcdef = const_pool_alloc(sizeof(Function));
   funcdef->argCount = array_list_size(func->parameters);
   funcdef->isNative = false;
   funcdef->funcBody.user.body = (BlockStmt*)func->body;
   funcdef->funcBody.user.args = func->parameters;
   funcdef->funcBody.user.varCount = func->varCount;
   hash_map_add_entry(interpret.functions, func->identifier, funcdef);
}


void create_native_func(HashMap *map, const char* name, int argCount, NativeFunc funcBody){
    Function* funcdef = const_pool_alloc(sizeof(Function));
    funcdef->isNative = true; 
    funcdef->argCount = argCount; 
    funcdef->funcBody.native = funcBody;
    hash_map_add_entry(map, string_from_const_str(name), funcdef);
}

void create_functions(uint32_t func_count){
    interpret.functions = hash_map_create((func_count + NATIVE_FUNC_COUNT) * 2, delete_functions);
    create_native_func(interpret.functions, "print", 1, print);
    create_native_func(interpret.functions, "abs", 1, _abs);
    create_native_func(interpret.functions, "bin", 1, bin);
    create_native_func(interpret.functions, "bool", 1, _bool);
    create_native_func(interpret.functions, "float", 1, _float);
    create_native_func(interpret.functions, "hex", 1, hex);
    create_native_func(interpret.functions, "input", 1, input);
    create_native_func(interpret.functions, "int", 1, _int);
    create_native_func(interpret.functions, "len", 1, len);
}

void interpretor_return(){
    Scope* local_scope = stack_peek(interpret.stackFrames, Scope*);
    longjmp(local_scope->start, 1);
}

void interpretor_call_function(String* name, Args* args){
    Function* func = hash_map_get_value(interpret.functions, name); 
    if(func == NULL) interpretor_throw_error("Function %s not defined\n", get_str(name));
    if(args->count != func->argCount){
        fprintf(stderr, "Invalid args expected %d, got %d\n", func->argCount, args->count);
        exit(1);
    }

    if(func->isNative){
        func->funcBody.native(args);
        return;
    }

    interpretor_create_scope(func->funcBody.user.varCount);
    Scope* local_scope = stack_peek(interpret.stackFrames, Scope*);
    if(setjmp(local_scope->start) == 0){
        //assign the args to the local scope 
        for(int i = 0; i < args->count; i++){
            String* arg_name = array_list_get(func->funcBody.user.args, String*, i);
            ClassInstance* value = interpretor_stack_pop();
            interpretor_assign_var(arg_name, value);
        }

        //execute the body of the function 
        ArrayList* body_stmts = func->funcBody.user.body->statements;
        for(int i = 0; i < array_list_size(body_stmts); i++){
            Statement* current_stmt = array_list_get(body_stmts,Statement*, i);
            interpret.currentStmt = current_stmt;
            evaluate_statement(current_stmt);
        }
    }     
    interpretor_delete_scope(); 
}



void interpretor_add_class(Class* obj){
    if(obj->isNative){
        hash_map_add_entry(interpret.classes, string_from_const_str(obj->native->name), obj);
    } else{
        hash_map_add_entry(interpret.classes, obj->user->name, obj);
    }
}


Class* interpretor_get_class(String* name){
    Class* c = hash_map_get_value(interpret.classes, name);
    if(c == NULL){
        interpretor_throw_error("Class %s not defined\n", get_str(name));
    }
    return c;
}


typedef struct {
    ArrayList args;
    BlockStmt* body;
} UserMethod;

typedef struct {
    int argCount;
    bool isNative;
    bool isSelf; //if the method takes self as a param
    union { 
        UserMethod user;
        NativeMethod native;
    } funcBody;
} Method;


static void delete_classes(void* class){
    Class* c = class;
    if(c->isNative){

    } else {
        UserClass* u = c->user;
        if(u->superClass != NULL){
            ArrayList* super = u->superClass;
            array_list_delete(super);
            free(u->superClass);
        }

        if(u->methods != NULL){
            hash_map_delete(u->methods);
        }

        if(u->staticVars != NULL){
            hash_map_delete(u->methods);
        }
        free(u);
    }
}


void interpt_stmts(ParserResult p_res){
    interpret.f = p_res.file;
    //call stack is going to be 255 for now
    stack_create(interpret.stackFrames, Scope*, MAX_SCOPE_DEPTH);
    stack_create(interpret.operandStack, ClassInstance, 10000);

    create_functions(p_res.funcDefc);

    interpret.classes = hash_map_create(20, delete_classes);

    create_int_class();
    create_float_class();
    create_none_class();
    create_str_class();

    interpretor_create_scope(p_res.varDefc);
    ArrayList* stmts = p_res.statements;

    for(int i = 0; i < array_list_size(stmts); i++){
        Statement* current_stmt = array_list_get(stmts, Statement*, i);
        interpret.currentStmt = current_stmt;
        evaluate_statement(current_stmt);
    }


    //cleanup 
    for(int i = 0; i < array_list_size(stmts); i++){
        Statement* current_stmt = array_list_get(stmts, Statement*, i);
        delete_statement(current_stmt);
    }


    array_list_delete(stmts);
    while(!stack_is_empty(interpret.stackFrames)){
        interpretor_delete_scope();
    }
    hash_map_delete(interpret.functions);
    hash_map_delete(interpret.classes);
}
