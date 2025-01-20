#include "array.h"
#include "object.h"
#include "interpret.h"
#include "stringtype.h"
#include "arena.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>




static void __add__(Args* args){
    ClassInstance* operand = interpretor_stack_pop();
    ClassInstance* self = interpretor_stack_pop();
    String* a = get_primitive(self);

    if(!is_str_class(operand)){
        interpretor_stack_push(NotImplemented);
        return;
    }
    String* b = get_primitive(operand); 
    String* concated_str = string_concat(a, b);
    new_str(concated_str);
}


static void __eq__(Args* args){ 
    ClassInstance* b = interpretor_stack_pop();
    ClassInstance* self = interpretor_stack_pop();
    //make sure they both are the same class first 
    if(b->classType != self->classType){
        new_bool(false); 
        return;
    }

    String* s1 = get_primitive(self);
    String* s2 = get_primitive(b);

    if(str_size(s1) != str_size(s2)){
        new_bool(false); 
        return;
    }
    new_bool(string_eq(s1,s2)); 
}


static void __bool__(Args* args){
    ClassInstance* self = interpretor_stack_pop();
    String* s = get_primitive(self);
    return new_bool(s != NULL && str_size(s) > 0);
}



static void __contains__(Args* args){
    ClassInstance* arg = interpretor_stack_pop();
    ClassInstance* self = interpretor_stack_pop();

    if(arg->classType != self->classType){
        interpretor_throw_error("Invalid type");
    }

    String* s1 = get_primitive(self);
    String* s2 = get_primitive(arg);
    new_bool(strstr(get_str(s1), get_str(s2)) != NULL);
}



static void __del__(Args* args){
    ClassInstance* self = interpretor_stack_pop();
    String* s = get_primitive(self);
    string_delete(s);
    interpretor_stack_push(None);
}



static void __float__(Args* args){
    ClassInstance* self = interpretor_stack_pop();
    String* s = get_primitive(self);
    errno = 0;
    char* end;
    double res = strtod(get_str(s), &end);
    if(errno == ERANGE || *end != '\0'){
        interpretor_throw_error("Invalid float");
    }

    new_float(res);
}



static void __ge__(Args* args){
    ClassInstance* arg = interpretor_stack_pop();
    ClassInstance* self = interpretor_stack_pop();

    if(arg->classType != self->classType){
        interpretor_throw_error("Invalid comparison");
    }

    String* s1 = get_primitive(self);
    String* s2 = get_primitive(arg);

    new_bool(strcmp(get_str(s1), get_str(s2)) >= 0);
}


static void __getitem__(Args* args){    
    interpretor_throw_error("Not implemented yet");
}



static void __int__(Args* args){
    ClassInstance* self = interpretor_stack_pop();
    String* s = get_primitive(self);
    errno = 0;
    char* end;
    long res = strtol(get_str(s), &end, 10);
    if(errno == ERANGE || *end != '\0'){
        interpretor_throw_error("Invalid integer");
    }
    new_integer(res);
}

static void __le__(Args* args){
    ClassInstance* arg = interpretor_stack_pop();
    ClassInstance* self = interpretor_stack_pop();

    if(arg->classType != self->classType){
        interpretor_throw_error("Invalid comparison");
    }

    String* s1 = get_primitive(self);
    String* s2 = get_primitive(arg);

    new_bool(strcmp(get_str(s1), get_str(s2)) <= 0);
}

static void __len__(Args* args){
    ClassInstance* self = interpretor_stack_pop();
    String* s = get_primitive(self);
    new_integer(str_size(s));
}

static void __lt__(Args* args){
    ClassInstance* arg = interpretor_stack_pop();
    ClassInstance* self = interpretor_stack_pop();

    if(arg->classType != self->classType){
        interpretor_throw_error("Invalid comparison");
    }

    String* s1 = get_primitive(self);
    String* s2 = get_primitive(arg);

    new_bool(strcmp(get_str(s1), get_str(s2)) < 0);
}

static void __mul__(Args* args){
    ClassInstance* arg = interpretor_stack_pop();
    ClassInstance* self = interpretor_stack_pop();

    if(!is_int_class(arg)){ 
        interpretor_throw_error("Invalid Type");
    }
    String* s1 = get_primitive(self);
    long* times = get_primitive(arg);
    String* result = string_multiply(s1, (uint32_t)(*times));
    new_str(result);
}

static void __ne__(Args* args) {
    __eq__( args);
    ClassInstance* b = interpretor_stack_peek();
    bool* equal = get_primitive(b);
    *equal = !(*equal);
}


static void __gt__(Args* args){
    ClassInstance* arg = interpretor_stack_pop();
    ClassInstance* self = interpretor_stack_pop();

    if(arg->classType != self->classType){
        interpretor_throw_error("Invalid comparison");
    }

    String* s1 = get_primitive(self);
    String* s2 = get_primitive(arg);

    new_bool(strcmp(get_str(s1), get_str(s2)) > 0);
}



static void __repr__(Args* args){
    ClassInstance* self = interpretor_stack_pop();
    String* s = get_primitive(self);
    new_str(string_copy(s));
}





void create_str_class(){
    PRIM_TYPE_STR.isNative = true;
    PRIM_TYPE_STR.native = const_pool_alloc(sizeof(NativeClass));
    PRIM_TYPE_STR.native->superClass = NULL; 
    PRIM_TYPE_STR.native->name = "str";
    PRIM_TYPE_STR.native->staticVars = NULL;
    PRIM_TYPE_STR.native->type = NATIVE_CLASS_STR;
    PRIM_TYPE_STR.native->methods = NULL; 
    PRIM_TYPE_STR.isMutable = false;
    ConstArray* list;
    const_array_create(list, NativeMethodInfo, 16);

    ADD_NATIVE_METHOD(list,__REPR__,__repr__, 1);
    ADD_NATIVE_METHOD(list,__ADD__,__add__, 2);
    ADD_NATIVE_METHOD(list,__EQ__,__eq__, 2);
    ADD_NATIVE_METHOD(list,__BOOL__,__bool__, 1);
    ADD_NATIVE_METHOD(list,__CONTAINS__,__contains__, 2);
    ADD_NATIVE_METHOD(list,__DEL__,__del__, 1);
    ADD_NATIVE_METHOD(list,__FLOAT__,__float__, 1);
    ADD_NATIVE_METHOD(list,__GE__,__ge__,2);
    ADD_NATIVE_METHOD(list,__GETITEM__,__getitem__, 2);
    ADD_NATIVE_METHOD(list,__INT__,__int__, 1);
    ADD_NATIVE_METHOD(list,__LE__,__le__, 2);
    ADD_NATIVE_METHOD(list,__LEN__,__len__, 1);
    ADD_NATIVE_METHOD(list,__LT__,__lt__, 2);
    ADD_NATIVE_METHOD(list,__MUL__,__mul__, 2);
    ADD_NATIVE_METHOD(list,__NE__,__ne__, 2);
    ADD_NATIVE_METHOD(list,__GT__,__gt__, 2);
    PRIM_TYPE_STR.native->methods = list;

    interpretor_add_class(&PRIM_TYPE_STR);
}




