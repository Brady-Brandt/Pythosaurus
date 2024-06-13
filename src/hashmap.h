#pragma once

//subject to change but I don't think I need anything larger
//this should be more than enough 
#define MAX_HASHMAP_SIZE 10000


typedef struct {
    char* key;
    void* value;
    struct HashEntry* next;
} HashEntry;

//hashmap uses seperate chaining for collisions 
typedef struct {
    HashEntry **data;
    unsigned int size;
    unsigned int capacity;
    //used for MAD compression
    unsigned int p;
    unsigned int a; 
    unsigned int b;
}HashMap;



HashMap hash_map_create(unsigned int capacity);


void hash_map_delete(HashMap* map);


//adds key, value to the hashmap 
//the value pointer must be allocated on the heap 
//the value pointer will be "owned" by the hashmap
void hash_map_add_entry(HashMap* map, const char* key, void* value);

//calls hash_map_add_entry, but mallocs a pointer to value first 
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
