#include "object.h"
#include "allocator.h"
#include "arraylist.h"
#include "hashmap.h"
#include "interpret.h"
#include "stringtype.h"

#include <string.h>
#include <limits.h>
#include <stdbool.h>
#include <time.h>

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
        return obj->user->name->str;
    }
} 


ClassInstance* call_native_method(struct Interpretor* interpret, ClassInstance* self, const char* name, MethodArgs* args){
    ArrayList methods = *self->classType->native->methods;
    for(int i = 0; i < methods.size; i++){
        NativeMethodInfo m = array_list_get(methods, NativeMethodInfo, i);
        //can just compare pointers 
        //since we use the same ptr for all native dunder methods 
        if(name == m.name){
            return m.method(interpret, args);
        }
    }

    if(self->classType->native->superClass != NULL){
        ArrayList super_class = *self->classType->native->superClass;

        for(int i =0; i < super_class.size; i++){
            Class* c = array_list_get(super_class, Class*, i);
            methods = *c->native->methods;
            for(int i = 0; i < methods.size; i++){
                NativeMethodInfo m = array_list_get(methods, NativeMethodInfo, i);
                if(name == m.name){
                    return m.method(interpret, args);
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


static ClassInstance* repr_none(struct Interpretor* interpret, MethodArgs* args){
    return new_str(interpret, string_from_str("None"));
}

static ClassInstance* eq_none(struct Interpretor* interpret, MethodArgs* args){
    return new_bool(interpret, __SELF__->classType == args->args[0]->classType);
}

static ClassInstance* ne_none(struct Interpretor* interpret, MethodArgs* args){
    return new_bool(interpret, __SELF__->classType != args->args[0]->classType);
}

static ClassInstance* hash_none(struct Interpretor* interpret, MethodArgs* args){
    return new_integer(interpret, (long)None);
}

static ClassInstance* repr_notimplemeted(struct Interpretor* interpret, MethodArgs* args){
    return new_str(interpret, string_from_str("NotImplemeted"));
}


void create_none_class(struct Interpretor* interpret){
    PRIM_TYPE_NONE.isNative = true;
    PRIM_TYPE_NONE.native = malloc(sizeof(NativeClass));
    PRIM_TYPE_NONE.native->superClass = NULL; 
    PRIM_TYPE_NONE.native->name = "none";
    PRIM_TYPE_NONE.native->staticVars = NULL;
    PRIM_TYPE_NONE.native->type = NATIVE_CLASS_NONE;
    PRIM_TYPE_NONE.native->methods = malloc(sizeof(ArrayList));
    PRIM_TYPE_NONE.isMutable = true; //technically not, but there should only be one None instance
    ArrayList none_list;
    array_list_create_cap(none_list, NativeMethodInfo, 4);

    ADD_NATIVE_METHOD(none_list, __REPR__, repr_none, 1, true);
    ADD_NATIVE_METHOD(none_list, __EQ__, eq_none, 2, true);
    ADD_NATIVE_METHOD(none_list, __NE__, ne_none, 2, true);
    ADD_NATIVE_METHOD(none_list, __HASH__, hash_none, 1, true);
   
    *PRIM_TYPE_NONE.native->methods = none_list;


    PRIM_TYPE_NOT_IMPLEMENTED.isNative = true;
    PRIM_TYPE_NOT_IMPLEMENTED.native = malloc(sizeof(NativeClass));
    PRIM_TYPE_NOT_IMPLEMENTED.native->superClass = NULL; 
    PRIM_TYPE_NOT_IMPLEMENTED.native->name = "notimplemented";
    PRIM_TYPE_NOT_IMPLEMENTED.native->staticVars = NULL;
    PRIM_TYPE_NOT_IMPLEMENTED.native->type = NATIVE_CLASS_NOT_IMPLEMENTED;
    PRIM_TYPE_NOT_IMPLEMENTED.native->methods = malloc(sizeof(ArrayList));

    PRIM_TYPE_NOT_IMPLEMENTED.isMutable = true; //same situation None
    ArrayList not_impl_list;

    array_list_create_cap(not_impl_list, NativeMethodInfo, 1);

    ADD_NATIVE_METHOD(not_impl_list, __REPR__, repr_notimplemeted, 1, true);
    
    *PRIM_TYPE_NOT_IMPLEMENTED.native->methods = not_impl_list;



    __None__.classType = &PRIM_TYPE_NONE;
    __None__.__dict__ = NULL;
    __None__.refCount = LONG_MAX;

    __NotImplemented__.classType = &PRIM_TYPE_NOT_IMPLEMENTED;
    __NotImplemented__.__dict__ = NULL;
    __NotImplemented__.refCount = LONG_MAX;


    interpretor_add_class((Interpretor*)interpret, &PRIM_TYPE_NONE);
    interpretor_add_class((Interpretor*)interpret, &PRIM_TYPE_NOT_IMPLEMENTED);
}



#define RETURN_NEW_IMMUTABLE_TYPE(interpret, val) \
    do { \
        Interpretor* inter = (Interpretor*)interpret; \
        allocator_add(&inter->expressionAllocator, val, ClassInstance); \
        return allocator_peek(&inter->expressionAllocator); \
    } while(0)

ClassInstance* new_float(struct Interpretor* interpret, double val){
    ClassInstance result = {0};
    result.pfloat = val;
    result.refCount = 1;
    result.classType = &PRIM_TYPE_FLOAT;
    RETURN_NEW_IMMUTABLE_TYPE(interpret, result);
}

ClassInstance* new_integer(struct Interpretor* interpret, long val){
    ClassInstance result = {0};
    result.pint = val;
    result.refCount = 1;
    result.classType = &PRIM_TYPE_INT;
    RETURN_NEW_IMMUTABLE_TYPE(interpret, result);
}


ClassInstance* new_bool(struct Interpretor* interpret, bool val){ 
    ClassInstance result = {0}; 
    result.pbool = val;
    result.refCount = 1;
    result.classType = &PRIM_TYPE_BOOL;
    RETURN_NEW_IMMUTABLE_TYPE(interpret, result);
}


ClassInstance* new_str(struct Interpretor* interpret, String* val){
    ClassInstance result = {0};
    result.pstr = val;
    result.refCount = 1;
    result.classType = &PRIM_TYPE_STR;
    RETURN_NEW_IMMUTABLE_TYPE(interpret, result);
}


void delete_native_class(NativeClass *class){
    if(class->superClass != NULL){
        ArrayList list = *class->superClass;
        array_list_delete(list);
        free(class->superClass);
    }
    ArrayList methods = *class->methods;
    array_list_delete(methods);
    free(class->methods);
    free(class);
}





