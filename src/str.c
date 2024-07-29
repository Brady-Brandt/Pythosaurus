#include "object.h"
#include "arraylist.h"
#include "interpret.h"
#include "stringtype.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>




static ClassInstance* __add__(struct Interpretor* interpret, MethodArgs* args){
   String* a = get_primitive(__SELF__);

   ClassInstance* operand = __M_ARG__(1);

   if(!is_str_class(operand)){
       return NotImplemented;
   }

   String* b = get_primitive(operand); 
   String* concated_str = string_concat(a, b);
   return  new_str(interpret, concated_str);
}


static ClassInstance* __eq__(struct Interpretor* interpret, MethodArgs* args){ 
   ClassInstance* b = __M_ARG__(1);
   //make sure they both are the same class first 
   if(b->classType != __SELF__->classType){
        return new_bool(interpret, false); 
   }

   String* s1 = get_primitive(__SELF__);
   String* s2 = get_primitive(b);

   if(s1->size != s2->size){
        return new_bool(interpret, false); 
   }
   return new_bool(interpret,strcmp(s1->str,s2->str) == 0); 
}


static ClassInstance* __bool__(struct Interpretor* interpret, MethodArgs* args){
   String* s = get_primitive(__SELF__);
   return new_bool(interpret, s != NULL && s->size > 0);
}



static ClassInstance* __contains__(struct Interpretor* interpret, MethodArgs* args){
    ClassInstance* arg = __M_ARG__(1);

    if(arg->classType != __SELF__->classType){
        interpretor_throw_error((Interpretor*)interpret, "Invalid type");
    }

    String* s1 = get_primitive(__SELF__);
    String* s2 = get_primitive(arg);

    return new_bool(interpret, strstr(s1->str, s2->str) != NULL);
}



static ClassInstance* __del__(struct Interpretor* interpret, MethodArgs* args){
    String* s = get_primitive(__SELF__);
    string_delete(s);
    free(__SELF__);
    return None;
}



static ClassInstance* __float__(struct Interpretor* interpret, MethodArgs* args){
    String* s = get_primitive(__SELF__);
    errno = 0;
    char* end;
    double res = strtod(s->str, &end);
    if(errno == ERANGE || *end != '\0'){
        interpretor_throw_error((Interpretor*)interpret, "Invalid float");
    }

    return new_float(interpret, res);
}



static ClassInstance* __ge__(struct Interpretor* interpret, MethodArgs* args){
    ClassInstance* arg = __M_ARG__(1);

    if(arg->classType != __SELF__->classType){
        interpretor_throw_error((Interpretor*)interpret, "Invalid comparison");
    }

    String* s1 = get_primitive(__SELF__);
    String* s2 = get_primitive(arg);

    return new_bool(interpret, strcmp(s1->str, s2->str) >= 0);
}


static ClassInstance* __getitem__(struct Interpretor* interpret, MethodArgs* args){
    ClassInstance* arg = __M_ARG__(1);
    if(!is_int_class(arg)){
        interpretor_throw_error((Interpretor*)interpret, "Indexing with invalid type");
    }
    long* temp = (long*)(get_primitive(arg));
    long index = *temp;
    String* s = get_primitive(__SELF__);
    if(index < 0 || index > s->size - 1){
        interpretor_throw_error((Interpretor*)interpret, "Index out of Range");
    }
    String* new_string = string_create_with_cap(1);
    string_push(&new_string, s->str[index]);
   
    return new_str(interpret, new_string);
}



static ClassInstance* __int__(struct Interpretor* interpret, MethodArgs* args){
    String* s = get_primitive(__SELF__);
    errno = 0;
    char* end;
    long res = strtol(s->str, &end, 10);
    if(errno == ERANGE || *end != '\0'){
        interpretor_throw_error((Interpretor*)interpret, "Invalid integer");
    }
    return new_integer(interpret, res);
}

static ClassInstance* __le__(struct Interpretor* interpret, MethodArgs* args){
    ClassInstance* arg = __M_ARG__(1);

    if(arg->classType != __SELF__->classType){
        interpretor_throw_error((Interpretor*)interpret, "Invalid comparison");
    }

    String* s1 = get_primitive(__SELF__);
    String* s2 = get_primitive(arg);

    return new_bool(interpret, strcmp(s1->str, s2->str) <= 0);
}

static ClassInstance* __len__(struct Interpretor* interpret, MethodArgs* args){
    String* s = get_primitive(__SELF__);
    return new_integer(interpret, s->size);
}

static ClassInstance* __lt__(struct Interpretor* interpret, MethodArgs* args){
    ClassInstance* arg = __M_ARG__(1);

    if(arg->classType != __SELF__->classType){
        interpretor_throw_error((Interpretor*)interpret, "Invalid comparison");
    }

    String* s1 = get_primitive(__SELF__);
    String* s2 = get_primitive(arg);

    return new_bool(interpret, strcmp(s1->str, s2->str) < 0);
}

static ClassInstance* __mul__(struct Interpretor* interpret, MethodArgs* args){
    ClassInstance* arg = __M_ARG__(1);

    if(!is_int_class(arg)){ 
        interpretor_throw_error((Interpretor*)interpret, "Invalid Type");
    }
    String* s1 = get_primitive(__SELF__);
    long* times = get_primitive(arg);
    String* result = string_multiply(s1, (uint32_t)(*times));
    return new_str(interpret, result);
}

static ClassInstance* __ne__(struct Interpretor* interpret, MethodArgs* args) {
    ClassInstance* b = __eq__(interpret,  args);
    bool* equal = get_primitive(b);
    *equal = !(*equal);
    return b;
}


static ClassInstance* __gt__(struct Interpretor* interpret, MethodArgs* args){
    ClassInstance* arg = __M_ARG__(1);

    if(arg->classType != __SELF__->classType){
        interpretor_throw_error((Interpretor*)interpret, "Invalid comparison");
    }

    String* s1 = get_primitive(__SELF__);
    String* s2 = get_primitive(arg);

    return new_bool(interpret, strcmp(s1->str, s2->str) > 0);
}



static ClassInstance* __repr__(struct Interpretor* interpret, MethodArgs* args){
    String* s = get_primitive(__SELF__);
    return new_str(interpret, string_copy(s));
}





void create_str_class(struct Interpretor* interpret){
    PRIM_TYPE_STR.isNative = true;
    PRIM_TYPE_STR.native = malloc(sizeof(NativeClass));
    PRIM_TYPE_STR.native->superClass = NULL; 
    PRIM_TYPE_STR.native->name = "str";
    PRIM_TYPE_STR.native->staticVars = NULL;
    PRIM_TYPE_STR.native->type = NATIVE_CLASS_STR;
    PRIM_TYPE_STR.native->methods = NULL; 
    PRIM_TYPE_STR.isMutable = false;
    ArrayList* list;
    array_list_create_cap(list, NativeMethodInfo, 50);

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

    interpretor_add_class((Interpretor*)interpret, &PRIM_TYPE_STR);
}




