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




static ClassInstance* __add__(MethodArgs* args){
   String* a = get_primitive(__SELF__);

   ClassInstance* operand = __M_ARG__(1);

   if(!is_str_class(operand)){
       return NotImplemented;
   }

   String* b = get_primitive(operand); 
   String* concated_str = string_concat(a, b);
   return  new_str(concated_str);
}


static ClassInstance* __eq__(MethodArgs* args){ 
   ClassInstance* b = __M_ARG__(1);
   //make sure they both are the same class first 
   if(b->classType != __SELF__->classType){
        return new_bool(false); 
   }

   String* s1 = get_primitive(__SELF__);
   String* s2 = get_primitive(b);

   if(str_size(s1) != str_size(s2)){
        return new_bool(false); 
   }
   return new_bool(string_eq(s1,s2)); 
}


static ClassInstance* __bool__(MethodArgs* args){
   String* s = get_primitive(__SELF__);
   return new_bool(s != NULL && str_size(s) > 0);
}



static ClassInstance* __contains__(MethodArgs* args){
    ClassInstance* arg = __M_ARG__(1);

    if(arg->classType != __SELF__->classType){
        interpretor_throw_error("Invalid type");
    }

    String* s1 = get_primitive(__SELF__);
    String* s2 = get_primitive(arg);

    return new_bool(strstr(get_str(s1), get_str(s2)) != NULL);
}



static ClassInstance* __del__(MethodArgs* args){
    String* s = get_primitive(__SELF__);
    string_delete(s);
    free(__SELF__);
    return None;
}



static ClassInstance* __float__(MethodArgs* args){
    String* s = get_primitive(__SELF__);
    errno = 0;
    char* end;
    double res = strtod(get_str(s), &end);
    if(errno == ERANGE || *end != '\0'){
        interpretor_throw_error("Invalid float");
    }

    return new_float(res);
}



static ClassInstance* __ge__(MethodArgs* args){
    ClassInstance* arg = __M_ARG__(1);

    if(arg->classType != __SELF__->classType){
        interpretor_throw_error("Invalid comparison");
    }

    String* s1 = get_primitive(__SELF__);
    String* s2 = get_primitive(arg);

    return new_bool(strcmp(get_str(s1), get_str(s2)) >= 0);
}


static ClassInstance* __getitem__(MethodArgs* args){
    ClassInstance* arg = __M_ARG__(1);
    if(!is_int_class(arg)){
        interpretor_throw_error("Indexing with invalid type");
    }
    long* temp = (long*)(get_primitive(arg));
    long index = *temp;
    String* s = get_primitive(__SELF__);
    if(index < 0 || index > str_size(s) - 1){
        interpretor_throw_error("Index out of Range");
    }
   

    interpretor_throw_error("Not implemented yet");
    return new_str(0);
}



static ClassInstance* __int__(MethodArgs* args){
    String* s = get_primitive(__SELF__);
    errno = 0;
    char* end;
    long res = strtol(get_str(s), &end, 10);
    if(errno == ERANGE || *end != '\0'){
        interpretor_throw_error("Invalid integer");
    }
    return new_integer(res);
}

static ClassInstance* __le__(MethodArgs* args){
    ClassInstance* arg = __M_ARG__(1);

    if(arg->classType != __SELF__->classType){
        interpretor_throw_error("Invalid comparison");
    }

    String* s1 = get_primitive(__SELF__);
    String* s2 = get_primitive(arg);

    return new_bool(strcmp(get_str(s1), get_str(s2)) <= 0);
}

static ClassInstance* __len__(MethodArgs* args){
    String* s = get_primitive(__SELF__);
    return new_integer(str_size(s));
}

static ClassInstance* __lt__(MethodArgs* args){
    ClassInstance* arg = __M_ARG__(1);

    if(arg->classType != __SELF__->classType){
        interpretor_throw_error("Invalid comparison");
    }

    String* s1 = get_primitive(__SELF__);
    String* s2 = get_primitive(arg);

    return new_bool(strcmp(get_str(s1), get_str(s2)) < 0);
}

static ClassInstance* __mul__(MethodArgs* args){
    ClassInstance* arg = __M_ARG__(1);

    if(!is_int_class(arg)){ 
        interpretor_throw_error("Invalid Type");
    }
    String* s1 = get_primitive(__SELF__);
    long* times = get_primitive(arg);
    String* result = string_multiply(s1, (uint32_t)(*times));
    return new_str(result);
}

static ClassInstance* __ne__(MethodArgs* args) {
    ClassInstance* b = __eq__( args);
    bool* equal = get_primitive(b);
    *equal = !(*equal);
    return b;
}


static ClassInstance* __gt__(MethodArgs* args){
    ClassInstance* arg = __M_ARG__(1);

    if(arg->classType != __SELF__->classType){
        interpretor_throw_error("Invalid comparison");
    }

    String* s1 = get_primitive(__SELF__);
    String* s2 = get_primitive(arg);

    return new_bool(strcmp(get_str(s1), get_str(s2)) > 0);
}



static ClassInstance* __repr__(MethodArgs* args){
    String* s = get_primitive(__SELF__);
    return new_str(string_copy(s));
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

    ADD_NATIVE_METHOD(list,__REPR__,__repr__, 1, true);
    ADD_NATIVE_METHOD(list,__ADD__,__add__, 2, true);
    ADD_NATIVE_METHOD(list,__EQ__,__eq__, 2, true);
    ADD_NATIVE_METHOD(list,__BOOL__,__bool__, 1, true);
    ADD_NATIVE_METHOD(list,__CONTAINS__,__contains__, 2, true);
    ADD_NATIVE_METHOD(list,__DEL__,__del__, 1, true);
    ADD_NATIVE_METHOD(list,__FLOAT__,__float__, 1, true);
    ADD_NATIVE_METHOD(list,__GE__,__ge__,2, true);
    ADD_NATIVE_METHOD(list,__GETITEM__,__getitem__, 2, true);
    ADD_NATIVE_METHOD(list,__INT__,__int__, 1, true);
    ADD_NATIVE_METHOD(list,__LE__,__le__, 2, true);
    ADD_NATIVE_METHOD(list,__LEN__,__len__, 1, true);
    ADD_NATIVE_METHOD(list,__LT__,__lt__, 2, true);
    ADD_NATIVE_METHOD(list,__MUL__,__mul__, 2, true);
    ADD_NATIVE_METHOD(list,__NE__,__ne__, 2, true);
    ADD_NATIVE_METHOD(list,__GT__,__gt__, 2, true);
    PRIM_TYPE_STR.native->methods = list;

    interpretor_add_class(&PRIM_TYPE_STR);
}




