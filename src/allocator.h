#pragma once
#include <stdlib.h>


typedef struct{
    long offset;
    long cap;
    size_t obj_size;
    void* data;
} Allocator;


void allocator_create(Allocator *alloc, size_t obj_size, long size);


void allocator_resize(Allocator *alloc, long new_size);

void* _allocator_add(Allocator *alloc);


void* allocator_peek(Allocator *alloc);

void allocator_clear(Allocator *alloc);

static inline long allocator_get_offset(Allocator *alloc){
    return alloc->offset;
}


static inline void allocator_set_offset(Allocator *alloc, long offset){
    alloc->offset = offset;
}

#define allocator_add(alloc_ptr, item, type) \
    do { \
        type* val = _allocator_add(alloc_ptr); \
        *val= item; \
    }while(0)



void* allocator_pop(Allocator *alloc);

void allocator_delete(Allocator *alloc);
