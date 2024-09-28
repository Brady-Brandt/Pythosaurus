#include "interpret.h"
#include "allocator.h"
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

//THESE ARE TEMP FUNCTIONS THAT WILL LIKELY GET DEPRECATED SOON
long interpretor_save_expression(){
    return allocator_get_offset(&interpret.expressionAllocator);
}

void interpretor_restore_expression(long offset){
    return allocator_set_offset(&interpret.expressionAllocator, offset);
}

void interpretor_set_stmt(Statement* stmt){
    interpret.currentStmt = stmt;
}

ClassInstance* interpretor_alloc_expr(ClassInstance instance){
    allocator_add(&interpret.expressionAllocator, instance, ClassInstance);
    return allocator_peek(&interpret.expressionAllocator);
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
    Allocator alloc;
    long expressionOffset;
} Scope;


void interpretor_create_scope(uint32_t size){
    if(size == 0){
        size = 10;
    }


    if(interpret.stackFrames->size == MAX_SCOPE_DEPTH - 1){
        interpretor_throw_error("Maximum scope depth of %d exceeded\n", MAX_SCOPE_DEPTH);
    }
    Scope* result = malloc(sizeof(Scope));
    if(result == NULL) interpretor_throw_error("Out of memory error\n");
    //create global scope 
    if(stack_is_empty(interpret.stackFrames)){
        allocator_create(&result->alloc, sizeof(ClassInstance), 32);
        result->variables = hash_map_create(size, delete_class_instance);
        result->expressionOffset = interpretor_save_expression();
        stack_push(interpret.stackFrames, Scope*, result);
    } else{
        allocator_create(&result->alloc, sizeof(ClassInstance), 10);
        result->variables = hash_map_create(size, delete_class_instance); 
        result->expressionOffset = interpretor_save_expression();
        stack_push(interpret.stackFrames, Scope*, result);
    }
}

void interpretor_delete_scope(){
    if(stack_is_empty(interpret.stackFrames)) return;
    Scope* s = stack_pop(interpret.stackFrames, Scope*);
    hash_map_delete(s->variables);
    interpretor_restore_expression(s->expressionOffset);
    allocator_delete(&s->alloc);

    if(s->variables != NULL){
        arena_pop(); //removes var data
        arena_pop(); //removes var hashmap 
    }
    
    free(s);
} 

void interpretor_assign_var(String* name, ClassInstance* value){
 
    //check if the variable exists first
    Scope* local_scope = stack_peek(interpret.stackFrames, Scope*);
    void* existing_value = hash_map_get_value(local_scope->variables, name);
    if(existing_value != NULL){
        memcpy(existing_value, value, sizeof(ClassInstance));
        return;
    }
    //if the variable doesn't exist we alloc a new spot for it 
    ClassInstance entry; 
    memcpy(&entry, value, sizeof(ClassInstance));
    allocator_add(&local_scope->alloc, entry, ClassInstance);
    ClassInstance* result = allocator_peek(&local_scope->alloc);
    hash_map_add_entry(local_scope->variables, name, result);
}



ClassInstance* interpretor_get_var(String* name){ 
    Scope* local_scope = stack_peek(interpret.stackFrames, Scope*);
    ClassInstance* value = hash_map_get_value(local_scope->variables, name);
    if(value != NULL && value != NotImplemented){ 
        return value;     
    } 
    interpretor_throw_error("Variable %s not defined\n", get_str(name));
}


void interpretor_global_var(String* name){
    Scope* local_scope = stack_peek(interpret.stackFrames, Scope*);
    ClassInstance* value = hash_map_get_value(local_scope->variables, name);
    if(value != NULL){
        interpretor_throw_error("%s assigned before global declaration", name);
    }

    Scope* global_scope = array_list_get(interpret.stackFrames, Scope*, 0);
    value = hash_map_get_value(global_scope->variables, name);

    //if the variable doesn't exist, declare in the global scope 
    if(value == NULL){
        allocator_add(&global_scope->alloc, *NotImplemented, ClassInstance);
        ClassInstance* result = allocator_peek(&global_scope->alloc);
        //add the value to global scope
        hash_map_add_entry(global_scope->variables, name, result);
        //add the value to the local scope 
        if(global_scope != local_scope) hash_map_add_entry(local_scope->variables, name, result);
    } else{ 
        //if the variable already exists in the local scope
        //add it to the local scope 
        hash_map_add_entry(local_scope->variables, name, value);
    }    
}

void interpretor_del_value(LiteralExpr* val){
    if(val->litType != LIT_IDENTIFIER){
        interpretor_throw_error("Cannot delete: %s", get_literal_type(val));
    }
    Scope* local_scope = stack_peek(interpret.stackFrames, Scope*);
    Scope* global_scope = array_list_get(interpret.stackFrames, Scope*, 0);
    ClassInstance* value = hash_map_delete_entry(local_scope->variables, val->identifier);

    if(local_scope != global_scope){
        ClassInstance* global_value = hash_map_get_value(global_scope->variables, val->identifier);
        //checks if the variable was declared with the global keyword 
        //it ensures the variable is deleted from the global scope as well as the local 
        if(global_value != NULL && global_value == value){
           hash_map_delete_entry(global_scope->variables, val->identifier); 
        }
    }

    if(value == NULL) interpretor_throw_error("%s not defined", get_str(val->identifier));
}


ClassInstance* func_args_get(FuncArgs* args, int index){
    if(index >= args->count) return NULL;
    return args->args[index];
}

void func_args_add(FuncArgs* args, ClassInstance* value){
    args->args[args->count++] = value;
}


typedef ClassInstance* (*NativeFunc)(FuncArgs*);

typedef struct {
    ArrayList* args;
    BlockStmt* body;
    int varCount;
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



void interpretor_create_function(FunctionStmt* func) {
   Function* funcdef = malloc(sizeof(Function));
   funcdef->argCount = array_list_size(func->parameters);
   funcdef->isNative = false;
   funcdef->funcBody.user.body = (BlockStmt*)func->body;
   funcdef->funcBody.user.args = func->parameters;
   funcdef->funcBody.user.varCount = func->varCount;
   hash_map_add_entry(interpret.functions, func->identifier, funcdef);
}


void create_native_func(HashMap *map, const char* name, int argCount, NativeFunc funcBody){
    Function* funcdef = malloc(sizeof(Function));
    funcdef->isNative = true; 
    funcdef->argCount = argCount; 
    funcdef->funcBody.native = funcBody;
    hash_map_add_entry(map, string_from_const_str(name), funcdef);
}

void create_functions(uint32_t func_count){
    interpret.functions = hash_map_create((func_count + NATIVE_FUNC_COUNT) * 2, delete_funcions);
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

void interpretor_return(ClassInstance* value){
    Scope* local_scope = stack_peek(interpret.stackFrames, Scope*);
    //bring the expression allocator back to where it was before we called the function 
    interpretor_restore_expression(local_scope->expressionOffset);
    //push the return value to the top of the expression stack 
    allocator_add(&interpret.expressionAllocator, *value, ClassInstance);
    //account for the new return value 
    //once the scope gets deleted the return value will be on top of the stack 
    local_scope->expressionOffset += interpret.expressionAllocator.obj_size;
    longjmp(local_scope->start, 1);
}

ClassInstance* interpretor_call_function(String* name, FuncArgs args){
    Function* func = hash_map_get_value(interpret.functions, name); 
    if(func == NULL) interpretor_throw_error("Function %s not defined\n", get_str(name));
    if(args.count != func->argCount){
        fprintf(stderr, "Invalid args expected %d, got %d\n", func->argCount, args.count);
        exit(1);
    }

    if(func->isNative){
        return func->funcBody.native(&args);
    }

    interpretor_create_scope(func->funcBody.user.varCount);
    Scope* local_scope = stack_peek(interpret.stackFrames, Scope*);
    ClassInstance* result = None;
    if(setjmp(local_scope->start) == 0){
        //assign the args to the local scope 
        for(int i = 0; i < args.count; i++){
            String* arg_name = array_list_get(func->funcBody.user.args, String*, i);
            ClassInstance* value = args.args[i];
            interpretor_assign_var(arg_name, value);
        }

        //execute the body of the function 
        ArrayList* body_stmts = func->funcBody.user.body->statements;
        for(int i = 0; i < array_list_size(body_stmts); i++){
            Statement* current_stmt = array_list_get(body_stmts,Statement*, i);
            interpret.currentStmt = current_stmt;
            evaluate_statement(current_stmt);
        }
    } else{
        //indicates an explicate return statement
        //return value is at the top of the expression allocator stack 
        result = allocator_peek(&interpret.expressionAllocator);
    }
    interpretor_delete_scope(); 
    return result;
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


void interpt_stmts(ParserResult p_res){
    interpret.f = p_res.file;
    //call stack is going to be 255 for now
    stack_create(interpret.stackFrames, Scope*, MAX_SCOPE_DEPTH);
    interpretor_create_scope(p_res.varDefc);
    allocator_create(&interpret.expressionAllocator, sizeof(ClassInstance), 64);
    allocator_create(&interpret.functionAllocator, sizeof(Function), 20);
    allocator_create(&interpret.methodAllocator, sizeof(Method), 100);
    create_functions(p_res.funcDefc);

    interpret.classes = hash_map_create(20, delete_classes);

    create_int_class();
    create_float_class();
    create_none_class();
    create_str_class();

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
    stack_delete(interpret.stackFrames);
    allocator_delete(&interpret.expressionAllocator);
    allocator_delete(&interpret.methodAllocator);
    allocator_delete(&interpret.functionAllocator);
    hash_map_delete(interpret.functions);
    hash_map_delete(interpret.classes);
}
