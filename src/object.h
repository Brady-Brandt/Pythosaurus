#pragma once 

#include "arraylist.h"
#include "array.h"
#include "hashmap.h"
#include "stringtype.h"
#include "statement.h"
#include <stdbool.h>
#include <stdint.h>


typedef struct {
    int count;
    //TODO: ADD kwargs and varargs
} Args;


typedef void (*NativeMethod)(Args*);

typedef void (*NativeFunc)(Args*);

typedef struct {
    ArrayList* args;
    BlockStmt* body;
    int varCount;
} UserFunc;

typedef struct {
    int argCount;
    bool isNative;
    union { 
        UserFunc user;
        NativeFunc native;
    } funcBody;
} Function;



//this defines methods for the builtin types 
typedef struct {
    const char* name;
    NativeMethod method;
    int argCount;
} NativeMethodInfo;



typedef enum {
    NATIVE_CLASS_INT,
    NATIVE_CLASS_BOOL,
    NATIVE_CLASS_STR,
    NATIVE_CLASS_FLOAT,
    NATIVE_CLASS_DICT,
    NATIVE_CLASS_LIST,
    NATIVE_CLASS_NONE,
    NATIVE_CLASS_NOT_IMPLEMENTED,
} NativeClassType;


typedef struct {
    NativeClassType type;
    const char* name;
    ConstArray* superClass;
    ConstArray* methods;
    ConstArray* staticVars;
} NativeClass;


typedef struct {
    String* name;
    ArrayList* superClass; //list of pointers to a superclass 
    HashMap* methods;  
    HashMap* staticVars;
} UserClass;


typedef struct {
    bool isNative;
    bool isMutable;
    union {
        NativeClass* native;
        UserClass* user;
    };
} Class;


//Define primitive types 
extern Class PRIM_TYPE_STR;
extern Class PRIM_TYPE_INT;
extern Class PRIM_TYPE_FLOAT;

extern Class PRIM_TYPE_BOOL;


extern Class PRIM_TYPE_DICT;
extern Class PRIM_TYPE_LIST;


extern Class PRIM_TYPE_NOT_IMPLEMENTED;
extern Class PRIM_TYPE_NONE;




typedef struct {
    Class* classType;
    union {
        HashMap* __dict__; //TODO: REPLACE WITH A DIFFERENT HASHMAP IMPLEMENTATION
        uint64_t pint;
        double pfloat;
        bool pbool;
        String* pstr;
    };
    uint64_t refCount; //use reference counting for gc, no need for atomics since its not multithreaded 
} ClassInstance;


static inline bool is_number_class(ClassInstance* inst){
    return inst->classType == &PRIM_TYPE_INT || inst->classType == &PRIM_TYPE_FLOAT || inst->classType == &PRIM_TYPE_BOOL;
}

static inline bool is_int_class(ClassInstance* inst){
    return inst->classType == &PRIM_TYPE_INT || inst->classType == &PRIM_TYPE_BOOL;
}

static inline bool is_str_class(ClassInstance* inst){
    return inst->classType == &PRIM_TYPE_STR;
}

static inline bool is_none_class(ClassInstance* inst){
    return inst->classType == &PRIM_TYPE_NONE;
}

static inline bool is_notimplemented_class(ClassInstance* inst){
    return inst->classType == &PRIM_TYPE_NOT_IMPLEMENTED;
}



extern ClassInstance __None__;
extern ClassInstance __NotImplemented__;

#define None (&__None__)
#define NotImplemented (&__NotImplemented__)


static inline void* get_primitive(ClassInstance* self){
   switch (self->classType->native->type) {
        case NATIVE_CLASS_INT:     
            return &self->pint;
        case NATIVE_CLASS_BOOL:
            return &self->pbool;
        case NATIVE_CLASS_FLOAT:
            return &self->pfloat;
        case NATIVE_CLASS_STR:
            return self->pstr;
        default:
            return None;
   } 
}


#define ADD_NATIVE_METHOD(list, str_name, func_ptr, arg_cnt) \
    do { \
        NativeMethodInfo *mi = &array_get(list, NativeMethodInfo, list->size++); \
        mi->name = str_name; \
        mi->method = func_ptr; \
        mi->argCount = arg_cnt; \
    } while(0)

extern const char* DUNDER_METHODS[73];
#define __ABS__            DUNDER_METHODS[0]
#define __ADD__            DUNDER_METHODS[1]
#define __AND__            DUNDER_METHODS[2]
#define __BOOL__           DUNDER_METHODS[3]
#define __BYTES__          DUNDER_METHODS[4]
#define __CEIL__           DUNDER_METHODS[5]
#define __CONTAINS__       DUNDER_METHODS[6]
#define __DEL__            DUNDER_METHODS[7]
#define __DELATTR__        DUNDER_METHODS[8]
#define __DELETE__         DUNDER_METHODS[9]
#define __DELITEM__        DUNDER_METHODS[10]
#define __DIR__            DUNDER_METHODS[11]
#define __DIVMOD__         DUNDER_METHODS[12]
#define __ENTER__          DUNDER_METHODS[13]
#define __EQ__             DUNDER_METHODS[14]
#define __EXIT__           DUNDER_METHODS[15]
#define __FLOAT__          DUNDER_METHODS[16]
#define __FLOOR__          DUNDER_METHODS[17]
#define __FLOORDIV__       DUNDER_METHODS[18]
#define __FORMAT__         DUNDER_METHODS[19]
#define __GE__             DUNDER_METHODS[20]
#define __GET__            DUNDER_METHODS[21]
#define __GETATTR__        DUNDER_METHODS[22]
#define __GETATTRIBUTE__   DUNDER_METHODS[23]
#define __GETITEM__        DUNDER_METHODS[24]
#define __GT__             DUNDER_METHODS[25]
#define __HASH__           DUNDER_METHODS[26]
#define __IADD__           DUNDER_METHODS[27]
#define __IAND__           DUNDER_METHODS[28]
#define __IFLOORDIV__      DUNDER_METHODS[29]
#define __ILSHIFT__        DUNDER_METHODS[30]
#define __IMOD__           DUNDER_METHODS[31]
#define __IMUL__           DUNDER_METHODS[32]
#define __INDEX__          DUNDER_METHODS[33]
#define __INIT__           DUNDER_METHODS[34]
#define __INSTANCECHECK__  DUNDER_METHODS[35]
#define __INT__            DUNDER_METHODS[36]
#define __INVERT__         DUNDER_METHODS[37]
#define __IOR__            DUNDER_METHODS[38]
#define __IPOW__           DUNDER_METHODS[39]
#define __IRSHIFT__        DUNDER_METHODS[40]
#define __ISUB__           DUNDER_METHODS[41]
#define __ITER__           DUNDER_METHODS[42]
#define __ITRUEDIV__       DUNDER_METHODS[43]
#define __IXOR__           DUNDER_METHODS[44]
#define __LE__             DUNDER_METHODS[45]
#define __LEN__            DUNDER_METHODS[46]
#define __LSHIFT__         DUNDER_METHODS[47]
#define __LT__             DUNDER_METHODS[48]
#define __MISSING__        DUNDER_METHODS[49]
#define __MOD__            DUNDER_METHODS[50]
#define __MUL__            DUNDER_METHODS[51]
#define __NE__             DUNDER_METHODS[52]
#define __NEG__            DUNDER_METHODS[53]
#define __NEW__            DUNDER_METHODS[54]
#define __NEXT__           DUNDER_METHODS[55]
#define __OR__             DUNDER_METHODS[56]
#define __POS__            DUNDER_METHODS[57]
#define __POW__            DUNDER_METHODS[58]
#define __REPR__           DUNDER_METHODS[59]
#define __REVERSED__       DUNDER_METHODS[60]
#define __ROUND__          DUNDER_METHODS[61]
#define __RSHIFT__         DUNDER_METHODS[62]
#define __SET__            DUNDER_METHODS[63]
#define __SET_NAME__       DUNDER_METHODS[64]
#define __SETATTR__        DUNDER_METHODS[65]
#define __SETITEM__        DUNDER_METHODS[66]
#define __STR__            DUNDER_METHODS[67]
#define __SUB__            DUNDER_METHODS[68]
#define __SUBCLASSCHECK__  DUNDER_METHODS[69]
#define __TRUEDIV__        DUNDER_METHODS[70]
#define __TRUNC__          DUNDER_METHODS[71]
#define __XOR__            DUNDER_METHODS[72]



const char* class_get_name(ClassInstance* self);


void create_int_class();

void create_str_class();

void create_float_class();

void create_none_class();

void delete_native_class(NativeClass *class);

void delete_class_instance(void* instance);

void call_native_method(const char* name, Args* args);

Class* class_new(String* name, ArrayList super_class);

void new_instance(Class* type, Args* args);

void new_float(double val);

void new_integer(long val);

void new_bool(bool val);

void new_str(String* val);
