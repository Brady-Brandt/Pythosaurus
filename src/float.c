#include "object.h"
#include "interpret.h"
#include "stringtype.h"

#include <math.h>


static ClassInstance* __abs__(struct Interpretor* interpret, MethodArgs* args){
    double* f = (double*)get_primitive(__SELF__);
    return new_float(interpret, fabs(*f));
}

static ClassInstance* __add__(struct Interpretor* interpret, MethodArgs* args){
    ClassInstance* arg = __M_ARG__(1); 
    if(!is_number_class(arg)){
        return NotImplemented;
    }
    double* a = (double*)get_primitive(__SELF__);
    double* b = (double*)get_primitive(arg);
    return new_float(interpret, *a + *b);
}

static ClassInstance* __bool__(struct Interpretor* interpret, MethodArgs* args){
    double* a = (double*)get_primitive(__SELF__);
    return new_bool(interpret, (bool)*a);
}

static ClassInstance* __ceil__(struct Interpretor* interpret, MethodArgs* args){
    double* a = (double*)get_primitive(__SELF__);
    return  new_integer(interpret, ceil(*a));
}

static ClassInstance* __divmod__(struct Interpretor* interpret, MethodArgs* args){ 
    interpretor_throw_error((Interpretor*)interpret, "divmod not implemeted yet for float");
}


static ClassInstance* __eq__(struct Interpretor* interpret, MethodArgs* args){
    ClassInstance* arg = __M_ARG__(1); 
    if(!is_number_class(arg)){
		return NotImplemented;
    }
    double* a = (double*)get_primitive(__SELF__);
    double* b = (double*)get_primitive(arg);
    return new_bool(interpret, *a == *b);
}


static ClassInstance* __float__(struct Interpretor* interpret, MethodArgs* args){
    double* a = (double*)get_primitive(__SELF__);
    return new_float(interpret, (double)*a);
}

static ClassInstance* __floor__(struct Interpretor* interpret, MethodArgs* args){
    double* a = (double*)get_primitive(__SELF__);
    return  new_integer(interpret, floor(*a));
}

static ClassInstance* __floordiv__(struct Interpretor* interpret, MethodArgs* args){
    ClassInstance* arg = __M_ARG__(1); 
    if(!is_number_class(arg)){
		return NotImplemented;
    }
    double* a = (double*)get_primitive(__SELF__);
    double* b = (double*)get_primitive(arg);
    return new_integer(interpret, *a / *b);
}

static ClassInstance* __format__(struct Interpretor* interpret, MethodArgs* args){
    interpretor_throw_error((Interpretor*)interpret, "format not implemeted yet for float");
}

static ClassInstance* __ge__(struct Interpretor* interpret, MethodArgs* args){
    ClassInstance* arg = __M_ARG__(1); 
    if(!is_number_class(arg)){
		return NotImplemented;
    }

    double* a = (double*)get_primitive(__SELF__);
    double* b = (double*)get_primitive(arg);
    return new_bool(interpret, *a >= *b);
}

static ClassInstance* __hash__(struct Interpretor* interpret, MethodArgs* args){
    interpretor_throw_error((Interpretor*)interpret, "hash not implemeted yet for float");
}


static ClassInstance* __int__(struct Interpretor* interpret, MethodArgs* args){
    double* a = (double*)get_primitive(__SELF__);
    return  new_integer(interpret, *a);
}

static ClassInstance* __le__(struct Interpretor* interpret, MethodArgs* args){
    ClassInstance* arg = __M_ARG__(1); 
    if(!is_number_class(arg)){
		return NotImplemented;
    }

    double* a = (double*)get_primitive(__SELF__);
    double* b = (double*)get_primitive(arg);
    return new_bool(interpret, *a <= *b);
}

static ClassInstance* __lt__(struct Interpretor* interpret, MethodArgs* args){
    ClassInstance* arg = __M_ARG__(1); 
    if(!is_number_class(arg)){
		return NotImplemented;
    }
    double* a = (double*)get_primitive(__SELF__);
    double* b = (double*)get_primitive(arg);
    return new_bool(interpret, *a < *b);
}


static ClassInstance* __mod__(struct Interpretor* interpret, MethodArgs* args){
    ClassInstance* arg = __M_ARG__(1); 
    if(!is_number_class(arg)){
		return NotImplemented;
    }
    double* a = (double*)get_primitive(__SELF__);
    double* b = (double*)get_primitive(arg);
    return new_float(interpret, fmod(*a, *b));
}


static ClassInstance* __mul__(struct Interpretor* interpret, MethodArgs* args){
    ClassInstance* arg = __M_ARG__(1); 
    if(!is_number_class(arg)){
		return NotImplemented;
    }
    double* a = (double*)get_primitive(__SELF__);
    double* b = (double*)get_primitive(arg);
    return new_float(interpret, *a * *b);
}

static ClassInstance* __ne__(struct Interpretor* interpret, MethodArgs* args){
    ClassInstance* arg = __M_ARG__(1); 
    if(!is_number_class(arg)){
		return NotImplemented;
    }
    double* a = (double*)get_primitive(__SELF__);
    double* b = (double*)get_primitive(arg);
    return new_float(interpret, *a != *b);
}


static ClassInstance* __neg__(struct Interpretor* interpret, MethodArgs* args){
    double* a = (double*)get_primitive(__SELF__);
    return  new_float(interpret, -(*a));
}

static ClassInstance* __pos__(struct Interpretor* interpret, MethodArgs* args){
    double* a = (double*)get_primitive(__SELF__);
    return  new_float(interpret, *a);
}

static ClassInstance* __pow__(struct Interpretor* interpret, MethodArgs* args){
    ClassInstance* arg = __M_ARG__(1); 
    if(!is_number_class(arg)){
		return NotImplemented;
    }
    double* a = (double*)get_primitive(__SELF__);
    double* b = (double*)get_primitive(arg);
    return new_float(interpret, pow(*a, *b));
}

static ClassInstance* __repr__(struct Interpretor* interpret, MethodArgs* args){ 
    double* a = (double*)get_primitive(__SELF__);
    char buffer[50];
    int size = snprintf(buffer, 50, "%g", *a);
    buffer[size] = '\0';
    String* res = string_from_str(buffer);
    return new_str(interpret, res);
}

static ClassInstance* __round__(struct Interpretor* interpret, MethodArgs* args){
    interpretor_throw_error((Interpretor*)interpret, "round not implemeted yet for float");
}

static ClassInstance* __gt__(struct Interpretor* interpret, MethodArgs* args){
    ClassInstance* arg = __M_ARG__(1); 
    if(!is_number_class(arg)){
		return NotImplemented;
    }
    double* a = (double*)get_primitive(__SELF__);
    double* b = (double*)get_primitive(arg);
    return new_bool(interpret, *a > *b);
}

static ClassInstance* __str__(struct Interpretor* interpret, MethodArgs* args){
    return __repr__(interpret, args);
}


static ClassInstance* __sub__(struct Interpretor* interpret, MethodArgs* args){
    ClassInstance* arg = __M_ARG__(1); 
    if(!is_number_class(arg)){
		return NotImplemented;
    }
    double* a = (double*)get_primitive(__SELF__);
    double* b = (double*)get_primitive(arg);
    return new_float(interpret, *a - *b);
}

static ClassInstance* __truediv__(struct Interpretor* interpret, MethodArgs* args){
    ClassInstance* arg = __M_ARG__(1); 
    if(!is_number_class(arg)){
		return NotImplemented;
    }
    double* a = (double*)get_primitive(__SELF__);
    double* b = (double*)get_primitive(arg);
    return new_float(interpret, (double)*a / (double)*b);
}

static ClassInstance* __trunc__(struct Interpretor* interpret, MethodArgs* args){
    double* a = (double*)get_primitive(__SELF__);
    return  new_float(interpret, *a);
}



void create_float_class(struct Interpretor* interpret){
    PRIM_TYPE_FLOAT.isNative = true;
    PRIM_TYPE_FLOAT.native = malloc(sizeof(NativeClass));
    PRIM_TYPE_FLOAT.native->superClass = NULL; 
    PRIM_TYPE_FLOAT.native->name = "float";
    PRIM_TYPE_FLOAT.native->staticVars = NULL;
    PRIM_TYPE_FLOAT.native->type = NATIVE_CLASS_FLOAT;
    PRIM_TYPE_FLOAT.native->methods = malloc(sizeof(ArrayList));
    PRIM_TYPE_FLOAT.isMutable = false;

    ArrayList list;
    array_list_create_cap(list, NativeMethodInfo, 40);    
    ADD_NATIVE_METHOD(list,__ABS__,__abs__,1, true);
    ADD_NATIVE_METHOD(list,__ADD__,__add__,2, true);
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
    ADD_NATIVE_METHOD(list,__INT__,__int__,1, true);
    ADD_NATIVE_METHOD(list,__LE__,__le__,2, true);
    ADD_NATIVE_METHOD(list,__LT__,__lt__,2, true);
    ADD_NATIVE_METHOD(list,__MOD__,__mod__,2, true);
    ADD_NATIVE_METHOD(list,__MUL__,__mul__,2, true);
    ADD_NATIVE_METHOD(list,__NE__,__ne__,2, true);
    ADD_NATIVE_METHOD(list,__NEG__,__neg__,1, true);
    ADD_NATIVE_METHOD(list,__POS__,__pos__,1, true);
    ADD_NATIVE_METHOD(list,__POW__,__pow__,2, true);
    ADD_NATIVE_METHOD(list,__REPR__,__repr__,1, true);
    ADD_NATIVE_METHOD(list,__ROUND__,__round__,2, true);
    ADD_NATIVE_METHOD(list,__GT__,__gt__,2, true);
    ADD_NATIVE_METHOD(list,__STR__,__str__,1, true);
    ADD_NATIVE_METHOD(list,__SUB__,__sub__,2, true);
    ADD_NATIVE_METHOD(list,__TRUEDIV__,__truediv__,2, true);
    ADD_NATIVE_METHOD(list,__TRUNC__,__trunc__,1, true);

    *PRIM_TYPE_FLOAT.native->methods = list;

    interpretor_add_class((Interpretor*)interpret, &PRIM_TYPE_FLOAT);
}

 
