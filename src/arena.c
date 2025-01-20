#include "arena.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <string.h>
#include <time.h>


typedef struct{
    uint64_t size; //how much data the arena currently holds
    uint64_t cap; //the total amount of data it can hold 
    uint8_t* block;
} Arena;


typedef struct {
    uint8_t* data;
    uint32_t size;
} BlockEntry;



//these are likely going to change 
#define SCRATCH_BUFFER_SIZE 100000
#define GLOBAL_SIZE_START 100000
#define CONST_SIZE_START 1000000




typedef struct {
    char* data;
    uint32_t offset;
} ScratchBuffer;

static ScratchBuffer scratch_buffer = {0};

static Arena global_arena = {0};
static Arena const_arena = {0};



noreturn static void alloc_error(){ 
    fprintf(stderr, "Failed to allocated memory\n");
    exit(EXIT_FAILURE);
}

void memory_init() {
    global_arena.cap = GLOBAL_SIZE_START;
    global_arena.size = 0;
    global_arena.block = malloc(global_arena.cap);
    
    const_arena.cap = CONST_SIZE_START;
    const_arena.size = 0;
    const_arena.block = malloc(const_arena.cap);

    
    scratch_buffer.data = malloc(SCRATCH_BUFFER_SIZE);
    scratch_buffer.offset = 0;

    if(const_arena.block == NULL || global_arena.block == NULL || scratch_buffer.data == NULL){
        alloc_error();
    } 
}


#define BLOCK_FOOTER sizeof(uint64_t)

//simple linear stack allocator
//this is only going to be used for objects 

void* arena_alloc(size_t size){
    if(global_arena.size + size + BLOCK_FOOTER > global_arena.cap){
        fprintf(stderr, "Out of memory\n");
        return NULL;
    }
    
    void* res = global_arena.block + global_arena.size;

    //going to store the size of the block after the data in a 32 bit integer 
    //this is so we can easily pop data of arbitary sizes out of the structure  
    *((uint64_t*)(res + size)) = size;
    global_arena.size += size + BLOCK_FOOTER;
    return res;
}

void* arena_peek(){
    if(global_arena.size == 0) return NULL;
    uint64_t data_size = *((uint64_t*)(global_arena.block + global_arena.size - BLOCK_FOOTER));
    return global_arena.block + global_arena.size - BLOCK_FOOTER - data_size;
}

void* arena_pop(){
    if(global_arena.size == 0) return NULL;
    uint64_t data_size = *((uint64_t*)(global_arena.block + global_arena.size - BLOCK_FOOTER));
    void* res = global_arena.block + global_arena.size - BLOCK_FOOTER - data_size;
    global_arena.size -= (data_size + BLOCK_FOOTER);
    return res;
}

uint64_t arena_get_offset(){
    return global_arena.size;
}

void arena_set_offset(uint64_t offset){
    global_arena.size = offset;
}



static void* _const_alloc(Arena* arena, size_t size){
    //just want a hard limit on the const pool for now 
    //eventually I will add an ability to extend the const pool 
    if(size + arena->size > arena->cap){
        fprintf(stderr, "Const pool is full\n");
        exit(EXIT_FAILURE);
    }

    void* res = arena->block + arena->size;
    arena->size += size;
    return res;
}

void* const_pool_alloc(size_t size){ 
   return _const_alloc(&const_arena, size); 
}



bool const_pool_contains_ptr(void* ptr){
    return ((uint8_t*)ptr >= const_arena.block) && (uint8_t*)ptr < (const_arena.block + const_arena.cap);
}


static inline void scratch_buffer_full(){
    if(scratch_buffer.offset >= SCRATCH_BUFFER_SIZE){
       fprintf(stderr, "ScratchBuffer is full"); 
       exit(EXIT_FAILURE);
    }
}

void scratch_buffer_append_char(char c){
    scratch_buffer_full();
    *((char*)scratch_buffer.data + scratch_buffer.offset) = c;
    scratch_buffer.offset += sizeof(char);
}


void scratch_buffer_clear(){
    scratch_buffer.offset = 0;
}


char* scratch_buffer_fmt(const char* fmt, ...){
    va_list list;
    va_start(list, fmt);
    uint32_t max_size = SCRATCH_BUFFER_SIZE - scratch_buffer.offset;
    vsnprintf(scratch_buffer.data + scratch_buffer.offset, max_size, fmt, list);
    va_end(list);
    return scratch_buffer.data;
}


char* scratch_buffer_vfmt(const char* fmt, va_list list){
    uint32_t max_size = SCRATCH_BUFFER_SIZE - scratch_buffer.offset;
    vsnprintf(scratch_buffer.data + scratch_buffer.offset, max_size, fmt, list);
    return scratch_buffer.data;
}


char* scratch_buffer_as_str(){ 
    *((char*)scratch_buffer.data + scratch_buffer.offset) = '\0';
    return scratch_buffer.data;
}

void* scratch_buffer_get_block(size_t size){
    void* res = (scratch_buffer.data + scratch_buffer.offset);
    scratch_buffer.offset += size;
    scratch_buffer_full();
    return res;
}

void scratch_buffer_pop_block(size_t size){
    if(size >= scratch_buffer.offset){
        scratch_buffer.offset = 0;
    } else{
        scratch_buffer.offset -= size;
    }

}


void memory_cleanup(){
    free(global_arena.block);
    free(const_arena.block);
    free(scratch_buffer.data);
}
