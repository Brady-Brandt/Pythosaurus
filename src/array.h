#pragma once
#include <stdint.h>

//arrays are going to be fixed sized heap allocated objects 
typedef struct {
    uint32_t size;
    uint32_t capacity;
    void* data;
} Array;



//Const arrays are going to be fixed sized heap allocated objects 
//this get allocated to the const pool 
typedef struct {
    uint32_t size;
    void* data;
} ConstArray;



#define array_size(arr) ((arr == NULL) ? 0 : arr->size)

#define const_array_create(arr, type, cap) \
do { \
    arr = const_pool_alloc(sizeof(ConstArray) + sizeof(type) * cap); \
    arr->size = 0; \
    arr->data = ((char*)arr + sizeof(ConstArray)); \
} while(0) 


#define const_array_append(arr, type, value) \
    do { \
        ((type*)arr->data)[arr->size++] = value; \
    } while(0)

#define array_get(arr, type, index)((type*)arr->data)[index]
