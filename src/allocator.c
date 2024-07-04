#include "allocator.h"
#include <stdlib.h>



void allocator_create(Allocator *alloc, size_t obj_size, long size){
    alloc->cap = size * obj_size;
    alloc->data = malloc(size * obj_size);
    alloc->offset = 0;
    alloc->obj_size = obj_size;
}


void allocator_resize(Allocator *alloc, long new_size){
    alloc->data = realloc(alloc->data, new_size);
    alloc->cap = new_size;
}


void* allocator_peek(Allocator *alloc){
    if(alloc->offset == 0) return NULL;
    return alloc->data + alloc->offset - alloc->obj_size;
}



void allocator_clear(Allocator *alloc){
    alloc->offset = 0;
}


void* _allocator_add(Allocator *alloc){
    if(alloc->offset + alloc->obj_size >= alloc->cap){
       allocator_resize(alloc, alloc->cap * 2); 
    }
    void* res = alloc->data + alloc->offset;
    alloc->offset += alloc->obj_size;
    return res;
}


void* allocator_pop(Allocator *alloc){
    if(alloc->offset == 0) return NULL; 
    alloc->offset -= alloc->obj_size;
    return alloc->data + alloc->offset;
}


void allocator_delete(Allocator *alloc){
    if(alloc->data != NULL){
        free(alloc->data);
        alloc->cap = 0;
        alloc->data = NULL;
    }
}

