#include "array.h"
#include "object.h"
#include "interpret.h"
#include "stringtype.h"
#include "arena.h"

#include <math.h>
#include <time.h>


static void __abs__(Args* args){
    ClassInstance* self = interpretor_stack_pop();
    double* f = (double*)get_primitive(self);
    new_float(fabs(*f));
}

static void __add__(Args* args){
    ClassInstance* arg = interpretor_stack_peek(); 
    if(!is_number_class(arg)){
        interpretor_stack_push(NotImplemented);
        return; 
    }

    arg = interpretor_stack_pop(); 
    ClassInstance* self = interpretor_stack_pop();
    double* a = (double*)get_primitive(self);
    double* b = (double*)get_primitive(arg);
    new_float(*a + *b);
}

static void __bool__(Args* args){
    ClassInstance* self = interpretor_stack_pop();
    double* a = (double*)get_primitive(self);
    new_bool((bool)*a);
}

static void __ceil__(Args* args){
    ClassInstance* self = interpretor_stack_pop();
    double* a = (double*)get_primitive(self);
    new_integer(ceil(*a));
}

static void __divmod__(Args* args){ 
    interpretor_throw_error("divmod not implemeted yet for float");
}


static void __eq__(Args* args){
    ClassInstance* arg = interpretor_stack_peek(); 
    if(!is_number_class(arg)){
		interpretor_stack_push(NotImplemented);
		return;
    }
    arg = interpretor_stack_pop(); 
    ClassInstance* self = interpretor_stack_pop();

    double* a = (double*)get_primitive(self);
    double* b = (double*)get_primitive(arg);
    new_bool(*a == *b);
}


static void __float__(Args* args){
    ClassInstance* self = interpretor_stack_pop();
    double* a = (double*)get_primitive(self);
    new_float((double)*a);
}

static void __floor__(Args* args){
    ClassInstance* self = interpretor_stack_pop();
    double* a = (double*)get_primitive(self);
    new_integer(floor(*a));
}

static void __floordiv__(Args* args){
    ClassInstance* arg = interpretor_stack_peek(); 
    if(!is_number_class(arg)){
		interpretor_stack_push(NotImplemented);
		return;
    }
    arg = interpretor_stack_pop(); 
    ClassInstance* self = interpretor_stack_pop();
    double* a = (double*)get_primitive(self);
    double* b = (double*)get_primitive(arg);
    new_integer(*a / *b);
}

static void __format__(Args* args){
    interpretor_throw_error("format not implemeted yet for float");
}

static void __ge__(Args* args){
    ClassInstance* arg = interpretor_stack_peek(); 
    if(!is_number_class(arg)){
		interpretor_stack_push(NotImplemented);
		return;
    }
    arg = interpretor_stack_pop(); 
    ClassInstance* self = interpretor_stack_pop();
    double* a = (double*)get_primitive(self);
    double* b = (double*)get_primitive(arg);
    new_bool(*a >= *b);
}

static void __hash__(Args* args){
    interpretor_throw_error("hash not implemeted yet for float");
}


static void __int__(Args* args){
    ClassInstance* self = interpretor_stack_pop();
    double* a = (double*)get_primitive(self);
    new_integer(*a);
}

static void __le__(Args* args){
    ClassInstance* arg = interpretor_stack_peek(); 
    if(!is_number_class(arg)){
		interpretor_stack_push(NotImplemented);
		return;
    }
    arg = interpretor_stack_pop(); 
    ClassInstance* self = interpretor_stack_pop();

    double* a = (double*)get_primitive(self);
    double* b = (double*)get_primitive(arg);
    new_bool(*a <= *b);
}

static void __lt__(Args* args){
    ClassInstance* arg = interpretor_stack_peek(); 
    if(!is_number_class(arg)){
		interpretor_stack_push(NotImplemented);
		return;
    }
    arg = interpretor_stack_pop(); 
    ClassInstance* self = interpretor_stack_pop();
    double* a = (double*)get_primitive(self);
    double* b = (double*)get_primitive(arg);
    new_bool(*a < *b);
}


static void __mod__(Args* args){
    ClassInstance* arg = interpretor_stack_peek(); 
    if(!is_number_class(arg)){
		interpretor_stack_push(NotImplemented);
		return;
    }
    arg = interpretor_stack_pop(); 
    ClassInstance* self = interpretor_stack_pop();

    double* a = (double*)get_primitive(self);
    double* b = (double*)get_primitive(arg);
    new_float(fmod(*a, *b));
}


static void __mul__(Args* args){
    ClassInstance* arg = interpretor_stack_peek(); 
    if(!is_number_class(arg)){
		interpretor_stack_push(NotImplemented);
		return;
    }
    arg = interpretor_stack_pop(); 
    ClassInstance* self = interpretor_stack_pop();

    double* a = (double*)get_primitive(self);
    double* b = (double*)get_primitive(arg);
    new_float(*a * *b);
}

static void __ne__(Args* args){
    ClassInstance* arg = interpretor_stack_peek(); 
    if(!is_number_class(arg)){
		interpretor_stack_push(NotImplemented);
		return;
    }
    arg = interpretor_stack_pop(); 
    ClassInstance* self = interpretor_stack_pop();

    double* a = (double*)get_primitive(self);
    double* b = (double*)get_primitive(arg);
    new_float(*a != *b);
}


static void __neg__(Args* args){
    ClassInstance* self = interpretor_stack_pop();
    double* a = (double*)get_primitive(self);
    new_float(-(*a));
}

static void __pos__(Args* args){
    ClassInstance* self = interpretor_stack_pop();
    double* a = (double*)get_primitive(self);
    new_float(*a);
}

static void __pow__(Args* args){
    ClassInstance* arg = interpretor_stack_peek(); 
    if(!is_number_class(arg)){
		interpretor_stack_push(NotImplemented);
		return;
    }
    arg = interpretor_stack_pop(); 
    ClassInstance* self = interpretor_stack_pop();

    double* a = (double*)get_primitive(self);
    double* b = (double*)get_primitive(arg);
    new_float(pow(*a, *b));
}

static void __repr__(Args* args){ 
    ClassInstance* self = interpretor_stack_pop();
    double* a = (double*)get_primitive(self);
    new_str(string_from_va("%g", *a));
}

static void __round__(Args* args){
    interpretor_throw_error("round not implemeted yet for float");
}

static void __gt__(Args* args){
    ClassInstance* arg = interpretor_stack_peek(); 
    if(!is_number_class(arg)){
		interpretor_stack_push(NotImplemented);
		return;
    }
    arg = interpretor_stack_pop(); 
    ClassInstance* self = interpretor_stack_pop();

    double* a = (double*)get_primitive(self);
    double* b = (double*)get_primitive(arg);
    new_bool(*a > *b);
}

static void __str__(Args* args){
    __repr__(args);
}


static void __sub__(Args* args){
    ClassInstance* arg = interpretor_stack_peek(); 
    if(!is_number_class(arg)){
		interpretor_stack_push(NotImplemented);
		return;
    }
    arg = interpretor_stack_pop(); 
    ClassInstance* self = interpretor_stack_pop();

    double* a = (double*)get_primitive(self);
    double* b = (double*)get_primitive(arg);
    new_float(*a - *b);
}

static void __truediv__(Args* args){
    ClassInstance* arg = interpretor_stack_peek(); 
    if(!is_number_class(arg)){
		interpretor_stack_push(NotImplemented);
		return;
    }
    arg = interpretor_stack_pop(); 
    ClassInstance* self = interpretor_stack_pop();

    double* a = (double*)get_primitive(self);
    double* b = (double*)get_primitive(arg);
    new_float((double)*a / (double)*b);
}

static void __trunc__(Args* args){
    ClassInstance* self = interpretor_stack_pop();
    double* a = (double*)get_primitive(self);
    new_float(*a);
}



void create_float_class(){
    PRIM_TYPE_FLOAT.isNative = true;
    PRIM_TYPE_FLOAT.native = const_pool_alloc(sizeof(NativeClass));
    PRIM_TYPE_FLOAT.native->superClass = NULL; 
    PRIM_TYPE_FLOAT.native->name = "float";
    PRIM_TYPE_FLOAT.native->staticVars = NULL;
    PRIM_TYPE_FLOAT.native->type = NATIVE_CLASS_FLOAT;
    PRIM_TYPE_FLOAT.native->methods = NULL;
    PRIM_TYPE_FLOAT.isMutable = false;

    ConstArray* list;
    const_array_create(list, NativeMethodInfo, 28);    
    ADD_NATIVE_METHOD(list,__ABS__,__abs__,1);
    ADD_NATIVE_METHOD(list,__ADD__,__add__,2);
    ADD_NATIVE_METHOD(list,__BOOL__,__bool__,1);
    ADD_NATIVE_METHOD(list,__CEIL__,__ceil__,1);
    ADD_NATIVE_METHOD(list,__DIVMOD__,__divmod__,2);
    ADD_NATIVE_METHOD(list,__EQ__,__eq__,2);
    ADD_NATIVE_METHOD(list,__FLOAT__,__float__,1);
    ADD_NATIVE_METHOD(list,__FLOOR__,__floor__,1);
    ADD_NATIVE_METHOD(list,__FLOORDIV__,__floordiv__,2);
    ADD_NATIVE_METHOD(list,__FORMAT__,__format__,2);
    ADD_NATIVE_METHOD(list,__GE__,__ge__,2);
    ADD_NATIVE_METHOD(list,__HASH__,__hash__,1);
    ADD_NATIVE_METHOD(list,__INT__,__int__,1);
    ADD_NATIVE_METHOD(list,__LE__,__le__,2);
    ADD_NATIVE_METHOD(list,__LT__,__lt__,2);
    ADD_NATIVE_METHOD(list,__MOD__,__mod__,2);
    ADD_NATIVE_METHOD(list,__MUL__,__mul__,2);
    ADD_NATIVE_METHOD(list,__NE__,__ne__,2);
    ADD_NATIVE_METHOD(list,__NEG__,__neg__,1);
    ADD_NATIVE_METHOD(list,__POS__,__pos__,1);
    ADD_NATIVE_METHOD(list,__POW__,__pow__,2);
    ADD_NATIVE_METHOD(list,__REPR__,__repr__,1);
    ADD_NATIVE_METHOD(list,__ROUND__,__round__,2);
    ADD_NATIVE_METHOD(list,__GT__,__gt__,2);
    ADD_NATIVE_METHOD(list,__STR__,__str__,1);
    ADD_NATIVE_METHOD(list,__SUB__,__sub__,2);
    ADD_NATIVE_METHOD(list,__TRUEDIV__,__truediv__,2);
    ADD_NATIVE_METHOD(list,__TRUNC__,__trunc__,1);

    PRIM_TYPE_FLOAT.native->methods = list;

    interpretor_add_class(&PRIM_TYPE_FLOAT);
}

 
