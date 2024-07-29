#include "interpret.h"
#include "allocator.h"
#include "arraylist.h"
#include "expression.h"
#include "file.h"
#include "hashmap.h"
#include "print.h"
#include "stack.h"
#include "statement.h"
#include "stringtype.h"
#include "evaluate.h"
#include "nativefunctions.h"
#include "object.h"

#include <setjmp.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>



#define MAX_SCOPE_DEPTH 255 

noreturn void interpretor_throw_error(Interpretor *interpret, const char* fmt, ...) {
    fprintf(stderr, "Error: \n");
    if(interpret->currentStmt != NULL){
        file_eprint_line(interpret->f, interpret->currentStmt->line);
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
    Allocator alloc;
    long expressionOffset;
} Scope;


void interpretor_create_scope(Interpretor* interpret){
    if(interpret->stackFrames->size == MAX_SCOPE_DEPTH - 1){
        interpretor_throw_error(interpret, "Maximum scope depth of %d exceeded\n", MAX_SCOPE_DEPTH);
    }
    Scope* result = malloc(sizeof(Scope));
    if(result == NULL) interpretor_throw_error(interpret, "Out of memory error\n");
    //create global scope 
    if(stack_is_empty(interpret->stackFrames)){
        allocator_create(&result->alloc, sizeof(ClassInstance), 32);
        result->variables = hash_map_create(32, delete_class_instance);
        result->expressionOffset = interpretor_save_expression(interpret);
        stack_push(interpret->stackFrames, Scope*, result);
    } else{
        allocator_create(&result->alloc, sizeof(ClassInstance), 10);
        result->variables = hash_map_create(10, delete_class_instance); 
        result->expressionOffset = interpretor_save_expression(interpret);
        stack_push(interpret->stackFrames, Scope*, result);
    }
}

void interpretor_delete_scope(Interpretor* interpret){
    if(stack_is_empty(interpret->stackFrames)) return;
    Scope* s = stack_pop(interpret->stackFrames, Scope*);
    hash_map_delete(s->variables);
    interpretor_restore_expression(interpret, s->expressionOffset);
    allocator_delete(&s->alloc);
    free(s);
} 

void interpretor_assign_var(Interpretor *interpret, String* name, ClassInstance* value){
 
    //check if the variable exists first
    Scope* local_scope = stack_peek(interpret->stackFrames, Scope*);
    void* existing_value = hash_map_get_value(local_scope->variables, name->str);
    if(existing_value != NULL){
        memcpy(existing_value, value, sizeof(ClassInstance));
        return;
    }
    //if the variable doesn't exist we alloc a new spot for it 
    ClassInstance entry; 
    memcpy(&entry, value, sizeof(ClassInstance));
    allocator_add(&local_scope->alloc, entry, ClassInstance);
    ClassInstance* result = allocator_peek(&local_scope->alloc);
    hash_map_add_entry(local_scope->variables, name->str, result);
}



ClassInstance* interpretor_get_var(Interpretor *interpret, String* name){ 
    Scope* local_scope = stack_peek(interpret->stackFrames, Scope*);
    ClassInstance* value = hash_map_get_value(local_scope->variables, name->str);
    if(value != NULL && value != NotImplemented){ 
        return value;     
    } 
    interpretor_throw_error(interpret, "Variable %s not defined\n", name->str);
}


void interpretor_global_var(Interpretor *interpret, String* name){
    Scope* local_scope = stack_peek(interpret->stackFrames, Scope*);
    ClassInstance* value = hash_map_get_value(local_scope->variables, name->str);
    if(value != NULL){
        interpretor_throw_error(interpret, "%s assigned before global declaration", name->str);
    }

    Scope* global_scope = array_list_get(interpret->stackFrames, Scope*, 0);
    value = hash_map_get_value(global_scope->variables, name->str);

    //if the variable doesn't exist, declare in the global scope 
    if(value == NULL){
        allocator_add(&global_scope->alloc, *NotImplemented, ClassInstance);
        ClassInstance* result = allocator_peek(&global_scope->alloc);
        //add the value to global scope
        hash_map_add_entry(global_scope->variables, name->str, result);
        //add the value to the local scope 
        if(global_scope != local_scope) hash_map_add_entry(local_scope->variables, name->str, result);
    } else{ 
        //if the variable already exists in the local scope
        //add it to the local scope 
        hash_map_add_entry(local_scope->variables, name->str, value);
    }    
}

void interpretor_del_value(Interpretor *interpret, LiteralExpr* val){
    if(val->litType != LIT_IDENTIFIER){
        interpretor_throw_error(interpret, "Cannot delete: %s", get_literal_type(val));
    }
    Scope* local_scope = stack_peek(interpret->stackFrames, Scope*);
    Scope* global_scope = array_list_get(interpret->stackFrames, Scope*, 0);
    ClassInstance* value = hash_map_delete_entry(local_scope->variables, val->identifier->str);

    if(local_scope != global_scope){
        ClassInstance* global_value = hash_map_get_value(global_scope->variables, val->identifier->str);
        //checks if the variable was declared with the global keyword 
        //it ensures the variable is deleted from the global scope as well as the local 
        if(global_value != NULL && global_value == value){
           hash_map_delete_entry(global_scope->variables, val->identifier->str); 
        }
    }

    if(value == NULL) interpretor_throw_error(interpret, "%s not defined", val->identifier->str);
}


ClassInstance* func_args_get(FuncArgs* args, int index){
    if(index >= args->count) return NULL;
    return args->args[index];
}

void func_args_add(FuncArgs* args, ClassInstance* value){
    args->args[args->count++] = value;
}


typedef ClassInstance* (*NativeFunc)(Interpretor*, FuncArgs*);

typedef struct {
    ArrayList* args;
    BlockStmt* body;
} UserFunc;

typedef struct {
    int argCount;
    bool isNative;
    union { 
        UserFunc user;
        NativeFunc native;
    } funcBody;
} Function;


static void delete_funcions(void* variable){
    Function* res = (Function*)variable;
    free(res); 
}



void interpretor_create_function(Interpretor *interpret, FunctionStmt* func) {
   Function* funcdef = malloc(sizeof(Function));
   funcdef->argCount = array_list_size(func->parameters);
   funcdef->isNative = false;
   funcdef->funcBody.user.body = (BlockStmt*)func->body;
   funcdef->funcBody.user.args = func->parameters;
   hash_map_add_entry(interpret->functions, func->identifier->str, funcdef);
}


void create_native_func(HashMap *map, const char* name, int argCount, NativeFunc funcBody){
    Function* funcdef = malloc(sizeof(Function));
    funcdef->isNative = true; 
    funcdef->argCount = argCount; 
    funcdef->funcBody.native = funcBody;
    hash_map_add_entry(map, name, funcdef);
}

void create_functions(Interpretor* interpret){
    interpret->functions = hash_map_create(30, delete_funcions);
    create_native_func(interpret->functions, "print", 1, print);
    create_native_func(interpret->functions, "abs", 1, _abs);
    create_native_func(interpret->functions, "bin", 1, bin);
    create_native_func(interpret->functions, "bool", 1, _bool);
    create_native_func(interpret->functions, "float", 1, _float);
    create_native_func(interpret->functions, "hex", 1, hex);
    create_native_func(interpret->functions, "input", 1, input);
    create_native_func(interpret->functions, "int", 1, _int);
    create_native_func(interpret->functions, "len", 1, len);
}

void interpretor_return(Interpretor* interpret, ClassInstance* value){
    Scope* local_scope = stack_peek(interpret->stackFrames, Scope*);
    //bring the expression allocator back to where it was before we called the function 
    interpretor_restore_expression(interpret, local_scope->expressionOffset);
    //push the return value to the top of the expression stack 
    allocator_add(&interpret->expressionAllocator, *value, ClassInstance);
    //account for the new return value 
    //once the scope gets deleted the return value will be on top of the stack 
    local_scope->expressionOffset += interpret->expressionAllocator.obj_size;
    longjmp(local_scope->start, 1);
}

ClassInstance* interpretor_call_function(Interpretor* interpret, String* name, FuncArgs args){
    Function* func = hash_map_get_value(interpret->functions, name->str); 
    if(func == NULL) interpretor_throw_error(interpret, "Function %s not defined\n", name->str);
    if(args.count != func->argCount){
        fprintf(stderr, "Invalid args expected %d, got %d\n", func->argCount, args.count);
        exit(1);
    }

    if(func->isNative){
        return func->funcBody.native(interpret, &args);
    }

    interpretor_create_scope(interpret);
    Scope* local_scope = stack_peek(interpret->stackFrames, Scope*);
    ClassInstance* result = None;
    if(setjmp(local_scope->start) == 0){
        //assign the args to the local scope 
        for(int i = 0; i < args.count; i++){
            String* arg_name = array_list_get(func->funcBody.user.args, String*, i);
            ClassInstance* value = args.args[i];
            interpretor_assign_var(interpret, arg_name, value);
        }

        //execute the body of the function 
        ArrayList* body_stmts = func->funcBody.user.body->statements;
        for(int i = 0; i < array_list_size(body_stmts); i++){
            Statement* current_stmt = array_list_get(body_stmts,Statement*, i);
            interpret->currentStmt = current_stmt;
            evaluate_statement(interpret, current_stmt);
        }
    } else{
        //indicates an explicate return statement
        //return value is at the top of the expression allocator stack 
        result = allocator_peek(&interpret->expressionAllocator);
    }
    interpretor_delete_scope(interpret); 
    return result;
}



void interpretor_add_class(Interpretor *interpret, Class* obj){
    if(obj->isNative){
        hash_map_add_entry(interpret->classes, obj->native->name, obj);
    } else{
        hash_map_add_entry(interpret->classes, obj->user->name->str, obj);
    }
}


Class* interpretor_get_class(Interpretor *interpret, String* name){
    Class* c = hash_map_get_value(interpret->classes, name->str);
    if(c == NULL){
        interpretor_throw_error(interpret, "Class %s not defined\n", name->str);
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
       delete_native_class(c->native); 
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


void interpt_stmts(File* file, ArrayList* stmts){
    Interpretor interpret = {0};
    interpret.f = file;
    //call stack is going to be 255 for now
    stack_create(interpret.stackFrames, Scope*, MAX_SCOPE_DEPTH);
    interpretor_create_scope(&interpret);
    allocator_create(&interpret.expressionAllocator, sizeof(ClassInstance), 64);
    allocator_create(&interpret.functionAllocator, sizeof(Function), 20);
    allocator_create(&interpret.methodAllocator, sizeof(Method), 100);
    create_functions(&interpret);

    interpret.classes = hash_map_create(20, delete_classes);

    create_int_class((struct Interpretor*)&interpret);
    create_float_class((struct Interpretor*)&interpret);
    create_none_class((struct Interpretor*)&interpret);
    create_str_class((struct Interpretor*)&interpret);


    for(int i = 0; i < array_list_size(stmts); i++){
        Statement* current_stmt = array_list_get(stmts, Statement*, i);
        interpret.currentStmt = current_stmt;
        evaluate_statement(&interpret, current_stmt);
    }





    //cleanup 
    for(int i = 0; i < array_list_size(stmts); i++){
        Statement* current_stmt = array_list_get(stmts, Statement*, i);
        delete_statement(current_stmt);
    }
    array_list_delete(stmts);
    while(!stack_is_empty(interpret.stackFrames)){
        interpretor_delete_scope(&interpret);
    }
    stack_delete(interpret.stackFrames);
    allocator_delete(&interpret.expressionAllocator);
    allocator_delete(&interpret.methodAllocator);
    allocator_delete(&interpret.functionAllocator);
    hash_map_delete(interpret.functions);
    hash_map_delete(interpret.classes);
}
