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



static ClassInstance* __abs__(MethodArgs* args){
    long* integer = (long*)get_primitive(__SELF__);
    return new_integer(labs(*integer));
}

static ClassInstance* __add__(MethodArgs* args){
    ClassInstance* arg = __M_ARG__(1); 
    if(!is_int_class(arg)){
        return NotImplemented;
    }
    long* a = (long*)get_primitive(__SELF__);
    long* b = (long*)get_primitive(arg);
    return new_integer(*a + *b);
}

static ClassInstance* __and__(MethodArgs* args){
    ClassInstance* arg = __M_ARG__(1); 
    if(!is_int_class(arg)){
        return NotImplemented;
    }
    long* a = (long*)get_primitive(__SELF__);
    long* b = (long*)get_primitive(arg);
    return new_integer(*a & *b);
}

static ClassInstance* __bool__(MethodArgs* args){
    long* a = (long*)get_primitive(__SELF__);
    return new_bool(*a);
}

static ClassInstance* __ceil__(MethodArgs* args){
    long* a = (long*)get_primitive(__SELF__);
    return  new_integer(*a);
}

static ClassInstance* __divmod__(MethodArgs* args){ 
    interpretor_throw_error("divmod not implemeted yet for int");
}


static ClassInstance* __eq__(MethodArgs* args){
    ClassInstance* arg = __M_ARG__(1); 
    if(!is_int_class(arg)){
		return NotImplemented;
    }
    long* a = (long*)get_primitive(__SELF__);
    long* b = (long*)get_primitive(arg);
    return new_bool(*a == *b);
}


static ClassInstance* __float__(MethodArgs* args){
    long* a = (long*)get_primitive(__SELF__);
    return new_float((double)*a);
}

static ClassInstance* __floor__(MethodArgs* args){
    long* a = (long*)get_primitive(__SELF__);
    return  new_integer(*a);
}

static ClassInstance* __floordiv__(MethodArgs* args){
    ClassInstance* arg = __M_ARG__(1); 
    if(!is_int_class(arg)){
		return NotImplemented;
    }
    long* a = (long*)get_primitive(__SELF__);
    long* b = (long*)get_primitive(arg);
    return new_integer(*a / *b);
}

static ClassInstance* __format__(MethodArgs* args){
    interpretor_throw_error("format not implemeted yet for int");
}

static ClassInstance* __ge__(MethodArgs* args){
    ClassInstance* arg = __M_ARG__(1); 
    if(!is_int_class(arg)){
		return NotImplemented;
    }

    long* a = (long*)get_primitive(__SELF__);
    long* b = (long*)get_primitive(arg);
    return new_bool(*a >= *b);
}

static ClassInstance* __hash__(MethodArgs* args){
    long* a = (long*)get_primitive(__SELF__);
    return  new_integer(*a);
}

static ClassInstance* __index__(MethodArgs* args){
    long* a = (long*)get_primitive(__SELF__);
    return  new_integer(*a);
}

static ClassInstance* __int__(MethodArgs* args){
    long* a = (long*)get_primitive(__SELF__);
    return  new_integer(*a);
}

static ClassInstance* __invert__(MethodArgs* args){
    long* a = (long*)get_primitive(__SELF__);
    return  new_integer(~(*a));
}

static ClassInstance* __le__(MethodArgs* args){
    ClassInstance* arg = __M_ARG__(1); 
    if(!is_int_class(arg)){
		return NotImplemented;
    }

    long* a = (long*)get_primitive(__SELF__);
    long* b = (long*)get_primitive(arg);
    return new_bool(*a <= *b);
}

static ClassInstance* __lshift__(MethodArgs* args){
    ClassInstance* arg = __M_ARG__(1); 
    if(!is_int_class(arg)){
		return NotImplemented;
    }
    long* a = (long*)get_primitive(__SELF__);
    long* b = (long*)get_primitive(arg);
    return new_integer(*a << *b);
}

static ClassInstance* __lt__(MethodArgs* args){
    ClassInstance* arg = __M_ARG__(1); 
    if(!is_int_class(arg)){
		return NotImplemented;
    }
    long* a = (long*)get_primitive(__SELF__);
    long* b = (long*)get_primitive(arg);
    return new_bool(*a < *b);
}


static ClassInstance* __mod__(MethodArgs* args){
    ClassInstance* arg = __M_ARG__(1); 
    if(!is_int_class(arg)){
		return NotImplemented;
    }
    long* a = (long*)get_primitive(__SELF__);
    long* b = (long*)get_primitive(arg);
    return new_integer(*a % *b);
}


static ClassInstance* __mul__(MethodArgs* args){
    ClassInstance* arg = __M_ARG__(1); 
    if(!is_int_class(arg)){
		return NotImplemented;
    }
    long* a = (long*)get_primitive(__SELF__);
    long* b = (long*)get_primitive(arg);
    return new_integer(*a * *b);
}

static ClassInstance* __ne__(MethodArgs* args){
    ClassInstance* arg = __M_ARG__(1); 
    if(!is_int_class(arg)){
		return NotImplemented;
    }
    long* a = (long*)get_primitive(__SELF__);
    long* b = (long*)get_primitive(arg);
    return new_bool(*a != *b);
}


static ClassInstance* __neg__(MethodArgs* args){
    long* a = (long*)get_primitive(__SELF__);
    return  new_integer(-(*a));
}



static ClassInstance* __or__(MethodArgs* args){
    ClassInstance* arg = __M_ARG__(1); 
    if(!is_int_class(arg)){
		return NotImplemented;
    }
    long* a = (long*)get_primitive(__SELF__);
    long* b = (long*)get_primitive(arg);
    return new_integer(*a | *b);
}


static ClassInstance* __pos__(MethodArgs* args){
    long* a = (long*)get_primitive(__SELF__);
    return  new_integer(*a);
}

static ClassInstance* __pow__(MethodArgs* args){
    ClassInstance* arg = __M_ARG__(1); 
    if(!is_int_class(arg)){
		return NotImplemented;
    }
    long* a = (long*)get_primitive(__SELF__);
    long* b = (long*)get_primitive(arg);
    return new_integer(powl(*a, *b));
}

static ClassInstance* __repr__(MethodArgs* args){ 
    long* a = (long*)get_primitive(__SELF__); 
    String* res = string_from_va("%ld", *a);
    return new_str(res);
}

static ClassInstance* __rshift__(MethodArgs* args){
    ClassInstance* arg = __M_ARG__(1); 
    if(!is_int_class(arg)){
		return NotImplemented;
    }
    long* a = (long*)get_primitive(__SELF__);
    long* b = (long*)get_primitive(arg);
    return new_integer(*a >> *b);
}



static ClassInstance* __round__(MethodArgs* args){
    interpretor_throw_error("round not implemeted yet for int");
}

static ClassInstance* __gt__(MethodArgs* args){
    ClassInstance* arg = __M_ARG__(1); 
    if(!is_int_class(arg)){
		return NotImplemented;
    }
    long* a = (long*)get_primitive(__SELF__);
    long* b = (long*)get_primitive(arg);
    return new_bool(*a > *b);
}

static ClassInstance* __str__(MethodArgs* args){
    return __repr__(args);
}


static ClassInstance* __sub__(MethodArgs* args){
    ClassInstance* arg = __M_ARG__(1); 
    if(!is_int_class(arg)){
		return NotImplemented;
    }
    long* a = (long*)get_primitive(__SELF__);
    long* b = (long*)get_primitive(arg);
    return new_integer(*a - *b);
}

static ClassInstance* __truediv__(MethodArgs* args){
    ClassInstance* arg = __M_ARG__(1); 
    if(!is_int_class(arg)){
		return NotImplemented;
    }
    long* a = (long*)get_primitive(__SELF__);
    long* b = (long*)get_primitive(arg);
    return new_integer((double)*a / (double)*b);
}

static ClassInstance* __trunc__(MethodArgs* args){
    long* a = (long*)get_primitive(__SELF__);
    return  new_integer(*a);
}


static ClassInstance* __xor__(MethodArgs* args){
    ClassInstance* arg = __M_ARG__(1); 
    if(!is_int_class(arg)){
		return NotImplemented;
    }
    long* a = (long*)get_primitive(__SELF__);
    long* b = (long*)get_primitive(arg);
    return new_integer(*a ^ *b);
}

//Going to define bool methods here to since its a subclass of integer and there aren't many methods 
static ClassInstance* bool_repr__(MethodArgs* args){
    bool* b = (bool*)get_primitive(__SELF__);
    return new_str(string_from_str((*b == true) ? "True" : "False"));
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

    ADD_NATIVE_METHOD(PRIM_TYPE_INT.native->methods,__ABS__,__abs__,1, true);
    ADD_NATIVE_METHOD(PRIM_TYPE_INT.native->methods,__ADD__,__add__,2, true);
    ADD_NATIVE_METHOD(PRIM_TYPE_INT.native->methods,__AND__,__and__,2, true);
    ADD_NATIVE_METHOD(PRIM_TYPE_INT.native->methods,__BOOL__,__bool__,1, true);
    ADD_NATIVE_METHOD(PRIM_TYPE_INT.native->methods,__CEIL__,__ceil__,1, true);
    ADD_NATIVE_METHOD(PRIM_TYPE_INT.native->methods,__DIVMOD__,__divmod__,2, true);
    ADD_NATIVE_METHOD(PRIM_TYPE_INT.native->methods,__EQ__,__eq__,2, true);
    ADD_NATIVE_METHOD(PRIM_TYPE_INT.native->methods,__FLOAT__,__float__,1, true);
    ADD_NATIVE_METHOD(PRIM_TYPE_INT.native->methods,__FLOOR__,__floor__,1, true);
    ADD_NATIVE_METHOD(PRIM_TYPE_INT.native->methods,__FLOORDIV__,__floordiv__,2, true);
    ADD_NATIVE_METHOD(PRIM_TYPE_INT.native->methods,__FORMAT__,__format__,2, true);
    ADD_NATIVE_METHOD(PRIM_TYPE_INT.native->methods,__GE__,__ge__,2, true);
    ADD_NATIVE_METHOD(PRIM_TYPE_INT.native->methods,__HASH__,__hash__,1, true);
    ADD_NATIVE_METHOD(PRIM_TYPE_INT.native->methods,__INDEX__,__index__,1, true);
    ADD_NATIVE_METHOD(PRIM_TYPE_INT.native->methods,__INT__,__int__,1, true);
    ADD_NATIVE_METHOD(PRIM_TYPE_INT.native->methods,__INVERT__,__invert__,1, true);
    ADD_NATIVE_METHOD(PRIM_TYPE_INT.native->methods,__LE__,__le__,2, true);
    ADD_NATIVE_METHOD(PRIM_TYPE_INT.native->methods,__LSHIFT__,__lshift__,2, true);
    ADD_NATIVE_METHOD(PRIM_TYPE_INT.native->methods,__LT__,__lt__,2, true);
    ADD_NATIVE_METHOD(PRIM_TYPE_INT.native->methods,__MOD__,__mod__,2, true);
    ADD_NATIVE_METHOD(PRIM_TYPE_INT.native->methods,__MUL__,__mul__,2, true);
    ADD_NATIVE_METHOD(PRIM_TYPE_INT.native->methods,__NE__,__ne__,2, true);
    ADD_NATIVE_METHOD(PRIM_TYPE_INT.native->methods,__NEG__,__neg__,1, true);
    ADD_NATIVE_METHOD(PRIM_TYPE_INT.native->methods,__OR__,__or__,2, true);
    ADD_NATIVE_METHOD(PRIM_TYPE_INT.native->methods,__POS__,__pos__,1, true); 
    ADD_NATIVE_METHOD(PRIM_TYPE_INT.native->methods,__POW__,__pow__,2, true);
    ADD_NATIVE_METHOD(PRIM_TYPE_INT.native->methods,__REPR__,__repr__,1, true);
    ADD_NATIVE_METHOD(PRIM_TYPE_INT.native->methods,__RSHIFT__,__rshift__,2, true);
    ADD_NATIVE_METHOD(PRIM_TYPE_INT.native->methods,__ROUND__,__round__,2, true);
    ADD_NATIVE_METHOD(PRIM_TYPE_INT.native->methods,__GT__,__gt__,2, true);
    ADD_NATIVE_METHOD(PRIM_TYPE_INT.native->methods,__STR__,__str__,1, true);
    ADD_NATIVE_METHOD(PRIM_TYPE_INT.native->methods,__SUB__,__sub__,2, true);
    ADD_NATIVE_METHOD(PRIM_TYPE_INT.native->methods,__TRUEDIV__,__truediv__,2, true);
    ADD_NATIVE_METHOD(PRIM_TYPE_INT.native->methods,__TRUNC__,__trunc__,1, true);
    ADD_NATIVE_METHOD(PRIM_TYPE_INT.native->methods,__XOR__,__xor__,2, true);  
    
    
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
    ADD_NATIVE_METHOD(bool_methods, __REPR__, bool_repr__, 1, true);
    ADD_NATIVE_METHOD(bool_methods, __STR__, bool_repr__, 1, true);

    PRIM_TYPE_BOOL.native->methods = bool_methods;

    interpretor_add_class(&PRIM_TYPE_INT);
    interpretor_add_class(&PRIM_TYPE_BOOL); 
}





