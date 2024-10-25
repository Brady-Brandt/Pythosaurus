#include "object.h"
#include "array.h"
#include "hashmap.h"
#include "interpret.h"
#include "stringtype.h"
#include "arena.h"

#include <string.h>
#include <limits.h>
#include <stdbool.h>

const char* DUNDER_METHODS[] = {
"__abs__",
"__add__",
"__and__",
"__bool__",
"__bytes__",
"__ceil__",
"__contains__",
"__del__",
"__delattr__",
"__delete__",
"__delitem__",
"__dir__",
"__divmod__",
"__enter__",
"__eq__",
"__exit__",
"__float__",
"__floor__",
"__floordiv__",
"__format__",
"__ge__",
"__get__",
"__getattr__",
"__getattribute__",
"__getitem__",
"__gt__",
"__hash__",
"__iadd__",
"__iand__",
"__ifloordiv__",
"__ilshift__",
"__imod__",
"__imul__",
"__index_",
"__init__",
"__instancecheck__",
"__int__",
"__invert__",
"__ior__",
"__ipow__",
"__irshift__",
"__isub__",
"__iter__",
"__itruediv__",
"__ixor__",
"__le__",
"__len__",
"__lshift__",
"__lt__",
"__missing__",
"__mod__",
"__mul__",
"__ne__",
"__neg__",
"__new__",
"__next__",
"__or__",
"__pos__",
"__pow__",
"__repr__",
"__reversed__",
"__round__",
"__rshift__",
"__set__",
"__set_name__",
"__setattr__",
"__setitem__",
"__str__",
"__sub__",
"__subclasscheck__",
"__truediv__",
"__trunc__",
"__xor__",
};


Class PRIM_TYPE_STR = {0};
Class PRIM_TYPE_INT = {0};
Class PRIM_TYPE_FLOAT = {0};

Class PRIM_TYPE_BOOL = {0};


Class PRIM_TYPE_DICT = {0};
Class PRIM_TYPE_LIST = {0};


Class PRIM_TYPE_NOT_IMPLEMENTED = {0};
Class PRIM_TYPE_NONE = {0};

ClassInstance  __None__;
ClassInstance  __NotImplemented__;




const char* class_get_name(ClassInstance* self){
    Class* obj = self->classType;
    if(obj->isNative){
        return obj->native->name;
    } else{
        return get_str(obj->user->name);
    }
} 


//this is called directly by the interpretor 
//for example a + b, if a & b are both primitive types, the interpretor will call this function 
//doing this we can guarentee that the char* passed in the function will be equal to the one 
//we have stored in our methods array 
//If the user calls the method a.__add__(b), the strings will be stored in different stops 
//so we can't compare pointers
//If users directly call dunder methods for primitive types, it will be slower than if the interpretor calls them
ClassInstance* call_native_method(ClassInstance* self, const char* name, MethodArgs* args){
    ConstArray* methods = self->classType->native->methods;
    for(int i = 0; i < array_size(methods); i++){
        NativeMethodInfo m = array_get(methods, NativeMethodInfo, i);
        //can just compare pointers 
        //since we use the same ptr for all native dunder methods 
        if(name == m.name){
            return m.method(args);
        }
    }

    if(self->classType->native->superClass != NULL){
        ConstArray* super_class = self->classType->native->superClass;

        for(int i = 0; i < array_size(super_class); i++){
            Class* c = array_get(super_class, Class*, i);
            methods = c->native->methods;
            for(int j = 0; j < array_size(methods); j++){
                NativeMethodInfo m = array_get(methods, NativeMethodInfo, j);
                if(name == m.name){
                    return m.method(args);
                }
            }
        }
    }
    return NotImplemented;
}


void delete_class_instance(void* instance){
    ClassInstance* cinst = (instance);
    cinst->refCount--;
    if(cinst->refCount == 0){
        if(cinst->classType->isNative){
            switch (cinst->classType->native->type) {
                case NATIVE_CLASS_STR:
                    string_delete(cinst->pstr);
                    break;
                    //bool, floats, ints do nothing 
                default:
                    break;
            }
        } else {
            //call __del__ method if there is one 
            if(cinst->__dict__ != NULL){
                hash_map_delete(cinst->__dict__);
            }
        }
    }
}


static ClassInstance* repr_none(MethodArgs* args){
    return new_str(string_from_str("None"));
}

static ClassInstance* eq_none(MethodArgs* args){
    return new_bool(__SELF__->classType == args->args[0]->classType);
}

static ClassInstance* ne_none(MethodArgs* args){
    return new_bool(__SELF__->classType != args->args[0]->classType);
}

static ClassInstance* hash_none(MethodArgs* args){
    return new_integer((long)None);
}

static ClassInstance* repr_notimplemeted(MethodArgs* args){
    return new_str(string_from_str("NotImplemeted"));
}


void create_none_class(){
    PRIM_TYPE_NONE.isNative = true;
    PRIM_TYPE_NONE.native = const_pool_alloc(sizeof(NativeClass));
    PRIM_TYPE_NONE.native->superClass = NULL; 
    PRIM_TYPE_NONE.native->name = "none";
    PRIM_TYPE_NONE.native->staticVars = NULL;
    PRIM_TYPE_NONE.native->type = NATIVE_CLASS_NONE;
    PRIM_TYPE_NONE.native->methods = NULL;
    PRIM_TYPE_NONE.isMutable = true; //technically not, but there should only be one None instance
    ConstArray* none_list;
    const_array_create(none_list, NativeMethodInfo, 4);

    ADD_NATIVE_METHOD(none_list, __REPR__, repr_none, 1, true);
    ADD_NATIVE_METHOD(none_list, __EQ__, eq_none, 2, true);
    ADD_NATIVE_METHOD(none_list, __NE__, ne_none, 2, true);
    ADD_NATIVE_METHOD(none_list, __HASH__, hash_none, 1, true);
   
    PRIM_TYPE_NONE.native->methods = none_list;


    PRIM_TYPE_NOT_IMPLEMENTED.isNative = true;
    PRIM_TYPE_NOT_IMPLEMENTED.native = const_pool_alloc(sizeof(NativeClass));
    PRIM_TYPE_NOT_IMPLEMENTED.native->superClass = NULL; 
    PRIM_TYPE_NOT_IMPLEMENTED.native->name = "notimplemented";
    PRIM_TYPE_NOT_IMPLEMENTED.native->staticVars = NULL;
    PRIM_TYPE_NOT_IMPLEMENTED.native->type = NATIVE_CLASS_NOT_IMPLEMENTED;
    PRIM_TYPE_NOT_IMPLEMENTED.native->methods = NULL;

    PRIM_TYPE_NOT_IMPLEMENTED.isMutable = true; //same situation None
    ConstArray* not_impl_list;

    const_array_create(not_impl_list, NativeMethodInfo, 1);

    ADD_NATIVE_METHOD(not_impl_list, __REPR__, repr_notimplemeted, 1, true);
    
    PRIM_TYPE_NOT_IMPLEMENTED.native->methods = not_impl_list;



    __None__.classType = &PRIM_TYPE_NONE;
    __None__.__dict__ = NULL;
    __None__.refCount = LONG_MAX;

    __NotImplemented__.classType = &PRIM_TYPE_NOT_IMPLEMENTED;
    __NotImplemented__.__dict__ = NULL;
    __NotImplemented__.refCount = LONG_MAX;


    interpretor_add_class(&PRIM_TYPE_NONE);
    interpretor_add_class(&PRIM_TYPE_NOT_IMPLEMENTED);
}



ClassInstance* new_float(double val){
    ClassInstance result = {0};
    result.pfloat = val;
    result.refCount = 1;
    result.classType = &PRIM_TYPE_FLOAT;
    return interpretor_alloc_expr(result);
}

ClassInstance* new_integer(long val){
    ClassInstance result = {0};
    result.pint = val;
    result.refCount = 1;
    result.classType = &PRIM_TYPE_INT;
    return interpretor_alloc_expr(result);
}


ClassInstance* new_bool(bool val){ 
    ClassInstance result = {0}; 
    result.pbool = val;
    result.refCount = 1;
    result.classType = &PRIM_TYPE_BOOL;
    return interpretor_alloc_expr(result);
}


ClassInstance* new_str(String* val){
    ClassInstance result = {0};
    result.pstr = val;
    result.refCount = 1;
    result.classType = &PRIM_TYPE_STR;
    return interpretor_alloc_expr(result);
}

