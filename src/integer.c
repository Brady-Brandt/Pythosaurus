#include "arraylist.h"
#include "object.h"
#include "stringtype.h"
#include "interpret.h"


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
    int len = 2;
    if(*a > 0){
        len += log10(*a);
    } else if(*a < 0){
        len += log10(labs(*a));
    }
    String* rep = string_create_with_cap(len);
    int size = snprintf(rep->str, len, "%ld", *a);
    rep->size = size;
    return new_str(rep);
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
    PRIM_TYPE_INT.native = malloc(sizeof(NativeClass));
    PRIM_TYPE_INT.native->superClass = NULL; 
    PRIM_TYPE_INT.native->name = "int";
    PRIM_TYPE_INT.native->staticVars = NULL;
    PRIM_TYPE_INT.native->type = NATIVE_CLASS_INT;
    PRIM_TYPE_INT.native->methods = NULL;
    PRIM_TYPE_INT.isMutable = false;
    ArrayList* list;
    array_list_create_cap(list, NativeMethodInfo, 50);

    ADD_NATIVE_METHOD(list,__ABS__,__abs__,1, true);
    ADD_NATIVE_METHOD(list,__ADD__,__add__,2, true);
    ADD_NATIVE_METHOD(list,__AND__,__and__,2, true);
    ADD_NATIVE_METHOD(list,__BOOL__,__bool__,1, true);
    ADD_NATIVE_METHOD(list,__CEIL__,__ceil__,1, true);
    ADD_NATIVE_METHOD(list,__DIVMOD__,__divmod__,2, true);
    ADD_NATIVE_METHOD(list,__EQ__,__eq__,2, true);
    ADD_NATIVE_METHOD(list,__FLOAT__,__float__,1, true);
    ADD_NATIVE_METHOD(list,__FLOOR__,__floor__,1, true);
    ADD_NATIVE_METHOD(list,__FLOORDIV__,__floordiv__,2, true);
    ADD_NATIVE_METHOD(list,__FORMAT__,__format__,2, true);
    ADD_NATIVE_METHOD(list,__GE__,__ge__,2, true);
    ADD_NATIVE_METHOD(list,__HASH__,__hash__,1, true);
    ADD_NATIVE_METHOD(list,__INDEX__,__index__,1, true);
    ADD_NATIVE_METHOD(list,__INT__,__int__,1, true);
    ADD_NATIVE_METHOD(list,__INVERT__,__invert__,1, true);
    ADD_NATIVE_METHOD(list,__LE__,__le__,2, true);
    ADD_NATIVE_METHOD(list,__LSHIFT__,__lshift__,2, true);
    ADD_NATIVE_METHOD(list,__LT__,__lt__,2, true);
    ADD_NATIVE_METHOD(list,__MOD__,__mod__,2, true);
    ADD_NATIVE_METHOD(list,__MUL__,__mul__,2, true);
    ADD_NATIVE_METHOD(list,__NE__,__ne__,2, true);
    ADD_NATIVE_METHOD(list,__NEG__,__neg__,1, true);
    ADD_NATIVE_METHOD(list,__OR__,__or__,2, true);
    ADD_NATIVE_METHOD(list,__POS__,__pos__,1, true);
    ADD_NATIVE_METHOD(list,__POW__,__pow__,2, true);
    ADD_NATIVE_METHOD(list,__REPR__,__repr__,1, true);
    ADD_NATIVE_METHOD(list,__RSHIFT__,__rshift__,2, true);
    ADD_NATIVE_METHOD(list,__ROUND__,__round__,2, true);
    ADD_NATIVE_METHOD(list,__GT__,__gt__,2, true);
    ADD_NATIVE_METHOD(list,__STR__,__str__,1, true);
    ADD_NATIVE_METHOD(list,__SUB__,__sub__,2, true);
    ADD_NATIVE_METHOD(list,__TRUEDIV__,__truediv__,2, true);
    ADD_NATIVE_METHOD(list,__TRUNC__,__trunc__,1, true);
    ADD_NATIVE_METHOD(list,__XOR__,__xor__,2, true);  

    PRIM_TYPE_INT.native->methods = list;

    PRIM_TYPE_BOOL.isNative = true;
    PRIM_TYPE_BOOL.native = malloc(sizeof(NativeClass));
    PRIM_TYPE_BOOL.native->superClass = NULL; 
    PRIM_TYPE_BOOL.native->name = "bool";
    PRIM_TYPE_BOOL.native->staticVars = NULL;
    PRIM_TYPE_BOOL.native->type = NATIVE_CLASS_BOOL;
    PRIM_TYPE_BOOL.native->methods = NULL;
    PRIM_TYPE_BOOL.isMutable = false;


    ArrayList* super_class;
    array_list_create_cap(super_class, Class*, 1);
    array_list_append(super_class, Class*, &PRIM_TYPE_INT);
    PRIM_TYPE_BOOL.native->superClass = super_class;

    ArrayList* bool_methods;
    array_list_create_cap(bool_methods, NativeMethodInfo, 5);
    ADD_NATIVE_METHOD(bool_methods, __REPR__, bool_repr__, 1, true);
    ADD_NATIVE_METHOD(bool_methods, __STR__, bool_repr__, 1, true);

    PRIM_TYPE_BOOL.native->methods = bool_methods;

    interpretor_add_class(&PRIM_TYPE_INT);
    interpretor_add_class(&PRIM_TYPE_BOOL);
}





