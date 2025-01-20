#include "array.h"
#include "object.h"
#include "stringtype.h"
#include "interpret.h"
#include "arena.h"


#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>



static void __abs__(Args* args){
    ClassInstance* self = interpretor_stack_pop();
    long* integer = (long*)get_primitive(self);
    new_integer(labs(*integer));
}

static void __add__(Args* args){
    ClassInstance* arg = interpretor_stack_peek(); 
    if(!is_int_class(arg)){
        interpretor_stack_push(NotImplemented);
        return;
    }
    
    arg = interpretor_stack_pop();
    ClassInstance* self = interpretor_stack_pop();

    long* a = (long*)get_primitive(self);
    long* b = (long*)get_primitive(arg);
    new_integer(*a + *b);
}

static void __and__(Args* args){
    ClassInstance* arg = interpretor_stack_peek(); 
    if(!is_int_class(arg)){
        interpretor_stack_push(NotImplemented);
        return;
    }
    arg = interpretor_stack_pop();
    ClassInstance* self = interpretor_stack_pop();

    long* a = (long*)get_primitive(self);
    long* b = (long*)get_primitive(arg);
    new_integer(*a & *b);
}

static void __bool__(Args* args){
    ClassInstance* self = interpretor_stack_pop();
    long* a = (long*)get_primitive(self);
    new_bool(*a);
}

static void __ceil__(Args* args){
    ClassInstance* self = interpretor_stack_pop();
    long* a = (long*)get_primitive(self);
     new_integer(*a);
}

static void __divmod__(Args* args){ 
    interpretor_throw_error("divmod not implemeted yet for int");
}


static void __eq__(Args* args){
    ClassInstance* arg = interpretor_stack_peek(); 
    if(!is_int_class(arg)){
        interpretor_stack_push(NotImplemented);
        return;
    }
    arg = interpretor_stack_pop();
    ClassInstance* self = interpretor_stack_pop();
    long* a = (long*)get_primitive(self);
    long* b = (long*)get_primitive(arg);
    new_bool(*a == *b);
}


static void __float__(Args* args){
    ClassInstance* self = interpretor_stack_pop();
    long* a = (long*)get_primitive(self);
    new_float((double)*a);
}

static void __floor__(Args* args){
    ClassInstance* self = interpretor_stack_pop();
    long* a = (long*)get_primitive(self);
     new_integer(*a);
}

static void __floordiv__(Args* args){
    ClassInstance* arg = interpretor_stack_peek(); 
    if(!is_int_class(arg)){
        interpretor_stack_push(NotImplemented);
        return;
    }
    arg = interpretor_stack_pop();
    ClassInstance* self = interpretor_stack_pop();

    long* a = (long*)get_primitive(self);
    long* b = (long*)get_primitive(arg);
    new_integer(*a / *b);
}

static void __format__(Args* args){
    interpretor_throw_error("format not implemeted yet for int");
}

static void __ge__(Args* args){
    ClassInstance* arg = interpretor_stack_peek(); 
    if(!is_int_class(arg)){
        interpretor_stack_push(NotImplemented);
        return;
    }
    arg = interpretor_stack_pop();
    ClassInstance* self = interpretor_stack_pop();
    
    long* a = (long*)get_primitive(self);
    long* b = (long*)get_primitive(arg);
    new_bool(*a >= *b);
}

static void __hash__(Args* args){
    ClassInstance* self = interpretor_stack_pop();
    long* a = (long*)get_primitive(self);
     new_integer(*a);
}

static void __index__(Args* args){
    ClassInstance* self = interpretor_stack_pop();
    long* a = (long*)get_primitive(self);
     new_integer(*a);
}

static void __int__(Args* args){
    ClassInstance* self = interpretor_stack_pop();
    long* a = (long*)get_primitive(self);
     new_integer(*a);
}

static void __invert__(Args* args){
    ClassInstance* self = interpretor_stack_pop();
    long* a = (long*)get_primitive(self);
     new_integer(~(*a));
}

static void __le__(Args* args){
    ClassInstance* arg = interpretor_stack_peek(); 
    if(!is_int_class(arg)){
		interpretor_stack_push(NotImplemented);
        return;
    }

    arg = interpretor_stack_pop();
    ClassInstance* self = interpretor_stack_pop();

    long* a = (long*)get_primitive(self);
    long* b = (long*)get_primitive(arg);
    new_bool(*a <= *b);
}

static void __lshift__(Args* args){
    ClassInstance* arg = interpretor_stack_peek(); 
    if(!is_int_class(arg)){
		interpretor_stack_push(NotImplemented);
        return;
    }

    arg = interpretor_stack_pop();
    ClassInstance* self = interpretor_stack_pop();
    long* a = (long*)get_primitive(self);
    long* b = (long*)get_primitive(arg);
    new_integer(*a << *b);
}

static void __lt__(Args* args){
    ClassInstance* arg = interpretor_stack_peek(); 
    if(!is_int_class(arg)){
		interpretor_stack_push(NotImplemented);
        return;
    }
    arg = interpretor_stack_pop();
    ClassInstance* self = interpretor_stack_pop();

    long* a = (long*)get_primitive(self);
    long* b = (long*)get_primitive(arg);
    new_bool(*a < *b);
}


static void __mod__(Args* args){
    ClassInstance* arg = interpretor_stack_peek(); 
    if(!is_int_class(arg)){
		interpretor_stack_push(NotImplemented);
        return;
    }
    arg = interpretor_stack_pop();
    ClassInstance* self = interpretor_stack_pop();

    long* a = (long*)get_primitive(self);
    long* b = (long*)get_primitive(arg);
    new_integer(*a % *b);
}


static void __mul__(Args* args){
    ClassInstance* arg = interpretor_stack_peek(); 
    if(!is_int_class(arg)){
		interpretor_stack_push(NotImplemented);
        return;
    }
    arg = interpretor_stack_pop();
    ClassInstance* self = interpretor_stack_pop();

    long* a = (long*)get_primitive(self);
    long* b = (long*)get_primitive(arg);
    new_integer(*a * *b);
}

static void __ne__(Args* args){
    ClassInstance* arg = interpretor_stack_peek(); 
    if(!is_int_class(arg)){
		interpretor_stack_push(NotImplemented);
        return;
    }

    arg = interpretor_stack_pop();
    ClassInstance* self = interpretor_stack_pop();

    long* a = (long*)get_primitive(self);
    long* b = (long*)get_primitive(arg);
    new_bool(*a != *b);
}


static void __neg__(Args* args){
    ClassInstance* self = interpretor_stack_pop();
    long* a = (long*)get_primitive(self);
     new_integer(-(*a));
}



static void __or__(Args* args){
    ClassInstance* arg = interpretor_stack_peek(); 
    if(!is_int_class(arg)){
		interpretor_stack_push(NotImplemented);
        return;
    }
    arg = interpretor_stack_pop();
    ClassInstance* self = interpretor_stack_pop();

    long* a = (long*)get_primitive(self);
    long* b = (long*)get_primitive(arg);
    new_integer(*a | *b);
}


static void __pos__(Args* args){
    ClassInstance* self = interpretor_stack_pop();
    long* a = (long*)get_primitive(self);
     new_integer(*a);
}

static void __pow__(Args* args){
    ClassInstance* arg = interpretor_stack_peek(); 
    if(!is_int_class(arg)){
		interpretor_stack_push(NotImplemented);
        return;
    }

    arg = interpretor_stack_pop();
    ClassInstance* self = interpretor_stack_pop();

    long* a = (long*)get_primitive(self);
    long* b = (long*)get_primitive(arg);
    new_integer(powl(*a, *b));
}

static void __repr__(Args* args){ 
    ClassInstance* self = interpretor_stack_pop();
    long* a = (long*)get_primitive(self); 
    String* res = string_from_va("%ld", *a);
    new_str(res);
}

static void __rshift__(Args* args){
    ClassInstance* arg = interpretor_stack_peek(); 
    if(!is_int_class(arg)){
		interpretor_stack_push(NotImplemented);
        return;
    }
    arg = interpretor_stack_pop();
    ClassInstance* self = interpretor_stack_pop();

    long* a = (long*)get_primitive(self);
    long* b = (long*)get_primitive(arg);
    new_integer(*a >> *b);
}



static void __round__(Args* args){
    interpretor_throw_error("round not implemeted yet for int");
}

static void __gt__(Args* args){
    ClassInstance* arg = interpretor_stack_peek(); 
    if(!is_int_class(arg)){
		interpretor_stack_push(NotImplemented);
        return;
    }
    arg = interpretor_stack_pop();
    ClassInstance* self = interpretor_stack_pop();

    long* a = (long*)get_primitive(self);
    long* b = (long*)get_primitive(arg);
    new_bool(*a > *b);
}

static void __str__(Args* args){
    __repr__(args);
}


static void __sub__(Args* args){
    ClassInstance* arg = interpretor_stack_peek(); 
    if(!is_int_class(arg)){
		interpretor_stack_push(NotImplemented);
        return;
    }

    arg = interpretor_stack_pop();
    ClassInstance* self = interpretor_stack_pop();

    long* a = (long*)get_primitive(self);
    long* b = (long*)get_primitive(arg);
    new_integer(*a - *b);
}

static void __truediv__(Args* args){
    ClassInstance* arg = interpretor_stack_peek(); 
    if(!is_int_class(arg)){
		interpretor_stack_push(NotImplemented);
        return;
    }

    arg = interpretor_stack_pop();
    ClassInstance* self = interpretor_stack_pop();

    long* a = (long*)get_primitive(self);
    long* b = (long*)get_primitive(arg);
    new_integer((double)*a / (double)*b);
}

static void __trunc__(Args* args){
    ClassInstance* self = interpretor_stack_pop();
    long* a = (long*)get_primitive(self);
     new_integer(*a);
}


static void __xor__(Args* args){
    ClassInstance* arg = interpretor_stack_peek(); 
    if(!is_int_class(arg)){
		interpretor_stack_push(NotImplemented);
        return;
    }
    arg = interpretor_stack_pop();
    ClassInstance* self = interpretor_stack_pop();

    long* a = (long*)get_primitive(self);
    long* b = (long*)get_primitive(arg);
    new_integer(*a ^ *b);
}

//Going to define bool methods here to since its a subclass of integer and there aren't many methods 
static void bool_repr__(Args* args){
    ClassInstance* self = interpretor_stack_pop();
    bool* b = (bool*)get_primitive(self);
    new_str(string_from_str((*b == true) ? "True" : "False"));
}



void create_int_class(){
    PRIM_TYPE_INT.isNative = true;
    PRIM_TYPE_INT.native = const_pool_alloc(sizeof(NativeClass));
    PRIM_TYPE_INT.native->superClass = NULL; 
    PRIM_TYPE_INT.native->name = "int";
    PRIM_TYPE_INT.native->staticVars = NULL;
    PRIM_TYPE_INT.native->type = NATIVE_CLASS_INT;
    PRIM_TYPE_INT.native->methods = NULL;
    PRIM_TYPE_INT.isMutable = false;
    PRIM_TYPE_INT.native->methods = NULL;
    const_array_create(PRIM_TYPE_INT.native->methods, NativeMethodInfo, 36);

    ADD_NATIVE_METHOD(PRIM_TYPE_INT.native->methods,__ABS__,__abs__,1);
    ADD_NATIVE_METHOD(PRIM_TYPE_INT.native->methods,__ADD__,__add__,2);
    ADD_NATIVE_METHOD(PRIM_TYPE_INT.native->methods,__AND__,__and__,2);
    ADD_NATIVE_METHOD(PRIM_TYPE_INT.native->methods,__BOOL__,__bool__,1);
    ADD_NATIVE_METHOD(PRIM_TYPE_INT.native->methods,__CEIL__,__ceil__,1);
    ADD_NATIVE_METHOD(PRIM_TYPE_INT.native->methods,__DIVMOD__,__divmod__,2);
    ADD_NATIVE_METHOD(PRIM_TYPE_INT.native->methods,__EQ__,__eq__,2);
    ADD_NATIVE_METHOD(PRIM_TYPE_INT.native->methods,__FLOAT__,__float__,1);
    ADD_NATIVE_METHOD(PRIM_TYPE_INT.native->methods,__FLOOR__,__floor__,1);
    ADD_NATIVE_METHOD(PRIM_TYPE_INT.native->methods,__FLOORDIV__,__floordiv__,2);
    ADD_NATIVE_METHOD(PRIM_TYPE_INT.native->methods,__FORMAT__,__format__,2);
    ADD_NATIVE_METHOD(PRIM_TYPE_INT.native->methods,__GE__,__ge__,2);
    ADD_NATIVE_METHOD(PRIM_TYPE_INT.native->methods,__HASH__,__hash__,1);
    ADD_NATIVE_METHOD(PRIM_TYPE_INT.native->methods,__INDEX__,__index__,1);
    ADD_NATIVE_METHOD(PRIM_TYPE_INT.native->methods,__INT__,__int__,1);
    ADD_NATIVE_METHOD(PRIM_TYPE_INT.native->methods,__INVERT__,__invert__,1);
    ADD_NATIVE_METHOD(PRIM_TYPE_INT.native->methods,__LE__,__le__,2);
    ADD_NATIVE_METHOD(PRIM_TYPE_INT.native->methods,__LSHIFT__,__lshift__,2);
    ADD_NATIVE_METHOD(PRIM_TYPE_INT.native->methods,__LT__,__lt__,2);
    ADD_NATIVE_METHOD(PRIM_TYPE_INT.native->methods,__MOD__,__mod__,2);
    ADD_NATIVE_METHOD(PRIM_TYPE_INT.native->methods,__MUL__,__mul__,2);
    ADD_NATIVE_METHOD(PRIM_TYPE_INT.native->methods,__NE__,__ne__,2);
    ADD_NATIVE_METHOD(PRIM_TYPE_INT.native->methods,__NEG__,__neg__,1);
    ADD_NATIVE_METHOD(PRIM_TYPE_INT.native->methods,__OR__,__or__,2);
    ADD_NATIVE_METHOD(PRIM_TYPE_INT.native->methods,__POS__,__pos__,1); 
    ADD_NATIVE_METHOD(PRIM_TYPE_INT.native->methods,__POW__,__pow__,2);
    ADD_NATIVE_METHOD(PRIM_TYPE_INT.native->methods,__REPR__,__repr__,1);
    ADD_NATIVE_METHOD(PRIM_TYPE_INT.native->methods,__RSHIFT__,__rshift__,2);
    ADD_NATIVE_METHOD(PRIM_TYPE_INT.native->methods,__ROUND__,__round__,2);
    ADD_NATIVE_METHOD(PRIM_TYPE_INT.native->methods,__GT__,__gt__,2);
    ADD_NATIVE_METHOD(PRIM_TYPE_INT.native->methods,__STR__,__str__,1);
    ADD_NATIVE_METHOD(PRIM_TYPE_INT.native->methods,__SUB__,__sub__,2);
    ADD_NATIVE_METHOD(PRIM_TYPE_INT.native->methods,__TRUEDIV__,__truediv__,2);
    ADD_NATIVE_METHOD(PRIM_TYPE_INT.native->methods,__TRUNC__,__trunc__,1);
    ADD_NATIVE_METHOD(PRIM_TYPE_INT.native->methods,__XOR__,__xor__,2);  
    
    
    PRIM_TYPE_BOOL.isNative = true;
    PRIM_TYPE_BOOL.native = const_pool_alloc(sizeof(NativeClass));   
    PRIM_TYPE_BOOL.native->superClass = NULL; 
    PRIM_TYPE_BOOL.native->name = "bool"; 
    PRIM_TYPE_BOOL.native->staticVars = NULL;
    PRIM_TYPE_BOOL.native->type = NATIVE_CLASS_BOOL;
    PRIM_TYPE_BOOL.native->methods = NULL;
    PRIM_TYPE_BOOL.isMutable = false;
    
    ConstArray* super_class;
    const_array_create(super_class, Class*, 1);
    const_array_append(super_class, Class*, &PRIM_TYPE_INT);
    PRIM_TYPE_BOOL.native->superClass = super_class;

    
    ConstArray* bool_methods;
    const_array_create(bool_methods, NativeMethodInfo, 2);
    ADD_NATIVE_METHOD(bool_methods, __REPR__, bool_repr__, 1);
    ADD_NATIVE_METHOD(bool_methods, __STR__, bool_repr__, 1);

    PRIM_TYPE_BOOL.native->methods = bool_methods;

    interpretor_add_class(&PRIM_TYPE_INT);
    interpretor_add_class(&PRIM_TYPE_BOOL); 
}





