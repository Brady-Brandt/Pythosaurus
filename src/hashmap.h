#pragma once

#include "stringtype.h"

//holds a function to determine how to delete each 
//valu in a hashmap 
typedef void (*delete_func)(void *);


//hashmap uses seperate chaining for collisions 
typedef struct {
    struct HashEntry **data;
    delete_func delete;
    unsigned int size;
    unsigned int capacity;
    //used for MAD compression
    unsigned int p;
    unsigned int a; 
    unsigned int b;
}HashMap;


//delete_func determines what to do when items are deleted from the hashmap 
//if null is inputted, it will just call free on the input data
HashMap* hash_map_create(unsigned int capacity, delete_func delete);


void hash_map_delete(HashMap* map);


//adds key, value to the hashmap 
//the value pointer must be allocated on the heap 
//the value pointer will be "owned" by the hashmap
void hash_map_add_entry(HashMap* map, String* key, void* value);


//deletes the given entry and returns it or null if it doesn't exist
void* hash_map_delete_entry(HashMap *map, String* key);

//deletes the given entry if it exists
void hash_map_delete_entry_with_value(HashMap *map, String* key);


//returns the value at the key or null
void* hash_map_get_value(HashMap *map, String* key);
