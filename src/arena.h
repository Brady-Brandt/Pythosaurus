#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>

void memory_init();

void* arena_alloc(size_t size);

void* arena_peek();

void* arena_pop();

uint64_t arena_get_offset();

void arena_set_offset(uint64_t offset);

void* const_pool_alloc(size_t size);

//returns true if the pointer was allocated from the const pool
bool const_pool_contains_ptr(void* ptr);

void scratch_buffer_append_char(char c);

char* scratch_buffer_fmt(const char* fmt, ...);

char* scratch_buffer_vfmt(const char* fmt, va_list list); 

void scratch_buffer_clear();

char* scratch_buffer_as_str();

void* scratch_buffer_get_block(size_t size);

//removes a block of size size from the buffer
void scratch_buffer_pop_block(size_t size);

void memory_cleanup();
