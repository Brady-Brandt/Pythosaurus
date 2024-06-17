#pragma once

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
HashMap hash_map_create(unsigned int capacity, delete_func delete);


void hash_map_delete(HashMap* map);


//adds key, value to the hashmap 
//the value pointer must be allocated on the heap 
//the value pointer will be "owned" by the hashmap
void hash_map_add_entry(HashMap* map, const char* key, void* value);

#define hash_map_add_kv(map_ptr, key, type, value) \
    do { \
        type* temp = malloc(sizeof(type)); \
        *temp = value; \
        hash_map_add_entry(map_ptr, key, temp); \
    } while(0)


//deletes the given entry and returns it or null if it doesn't exist
void* hash_map_delete_entry(HashMap *map, char* key);

//deletes the given entry if it exists
void hash_map_delete_entry_with_value(HashMap *map, char* key);


//returns the value at the key or null
void* hash_map_get_value(HashMap *map, char* key);
