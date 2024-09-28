#include "hashmap.h"
#include "stringtype.h"
#include "arena.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>



//subject to change but I don't think I need anything larger
//this should be more than enough 
#define MAX_HASHMAP_SIZE 10000


//mad compression requires a prime number greater than capacity
#define LARGEST_PRIME 10007


typedef struct {
    String* key;
    void* value;
    struct HashEntry* next;
} HashEntry;



static unsigned int find_prime(unsigned int cap){
    //make sure cap is odd so we can skip over all evens
    if(cap < 5){
        cap = 7;
    }
    if(cap % 2 == 0) cap++;
    for(int i = cap; i < LARGEST_PRIME; i+=2){
        bool isprime = true;
        for(int j = 2; j < sqrt(cap) + 1; j++){
            if(i % j == 0) {
                isprime = false;
                break;
            }
        }
        if(isprime) return i;
    }
    return 0;
}


HashMap* hash_map_create(unsigned int capacity, delete_func delete){
    if(capacity == 0 || capacity > MAX_HASHMAP_SIZE){
        fprintf(stderr, "Failed to create hashmap invalid size");
        return NULL;
    }
    HashMap* result = arena_alloc(sizeof(HashMap));
    if(result == NULL) return result;
    result->capacity = capacity;
    result->size = 0;
    result->delete = delete;
    result->data = NULL;
    result->data = arena_alloc(sizeof(HashEntry*) * capacity); 
    memset(result->data, 0, sizeof(HashEntry*) * capacity);
    result->p = find_prime(result->capacity);
    result->a = rand() % (result->p - 1) + 1;
    result->b = rand() % (result->p - 1);
    return result;
}



void hash_map_delete(HashMap* map){
    for(int i = 0; i < map->capacity; i++){
        HashEntry* curr = (HashEntry*)map->data[i];
        while(curr != NULL){
            HashEntry* temp = (HashEntry*)curr->next;
            if(map->delete == NULL){
                //free the value
                free(curr->value);
            } else {
                map->delete(curr->value);
            }
            //free the entry 
            free(curr);
            map->size--;
            curr = temp;
            
        }
        if(map->size == 0) break;
    }
}

//performs cyclic shift has function
static unsigned long hash(String* key) {
    int shift = 27;
    int hash = 0;
    int size = str_size(key);
    char* data = get_str(key);
    for(int i = 0; i < size; i++){
        hash = (hash<<(shift)) | (hash>>(32-shift));
        hash += (int)(data[i]);
    }
    return hash;
}

//performs MAD compression 
static unsigned int compression(HashMap* map, unsigned long hash){
    return (((map->a * hash + map->b) % map->p ) % map->capacity);   
}



static void add_entry(HashMap *map, unsigned int index, HashEntry* entry){
    //just put it at the index if it is empty
    if(map->data[index] == NULL){
       map->data[index] = (struct HashEntry*)entry;
       map->size++;
    } else {
        //perform seperate chaining
        HashEntry* curr = (HashEntry*)map->data[index];
        HashEntry* prev = NULL;
        while(curr != NULL){ 
            //if this key already exists in the list we change the value at the key 
            if(string_eq(entry->key, curr->key)){
                if(map->delete != NULL){
                    map->delete(curr->value); 
                } else{
                    free(curr->value);
                }
                entry->next = curr->next;
                //replacing the value at the start of the list
                if(prev == NULL){
                    map->data[index] = (struct HashEntry*)entry;
                } else{
                    prev->next = (struct HashEntry*)entry;
                }
                //free the entry pointer
                free(curr);
                return;
            }
            if(curr->next == NULL){
                curr->next = (struct HashEntry*)entry;
                break;
            }
            prev = curr;
            curr = (HashEntry*)curr->next; 
        }
        map->size++;
    }
}



void hash_map_add_entry(HashMap* map, String* key, void* value){
    //an expensize operation so try to avoid 
    if(map->size == map->capacity){
        fprintf(stderr, "Warning: Hashmap is full\n");
    }

    unsigned int index = compression(map, hash(key));
    HashEntry* entry = malloc(sizeof(HashEntry));
    entry->key = key;
    entry->value = value;
    entry->next = NULL;
    add_entry(map, index, entry); 
}


void* hash_map_delete_entry(HashMap *map, String* key){
    unsigned int index = compression(map, hash(key));
    HashEntry* curr = (HashEntry*)map->data[index];
    HashEntry* prev = curr;
    while(curr != NULL) {
        //ensure the keys match 
        if(string_eq(key, curr->key)){
            //remove the index of from front or end of linked list
            if(prev != curr){
                prev->next = curr->next;
            }
            else{
                //removing the index from the front of the linked list
                map->data[index] = curr->next;
            }
            void* value = curr->value;
            free(curr);
            curr = NULL;
            map->size--;
            return value;
        }
        curr = (HashEntry*)curr->next;
        prev = curr;
    }
    return NULL;

}

void hash_map_delete_entry_with_value(HashMap *map, String* key){
    void* data = hash_map_delete_entry(map, key);
    if(data != NULL) {
        if(map->delete != NULL){
            map->delete(data);
        } else{ 
            free(data);
        }
    }
}

void* hash_map_get_value(HashMap *map, String* key){
    unsigned int index = compression(map, hash(key));
    HashEntry* curr = (HashEntry*)map->data[index];
    while(curr != NULL) {
        if(string_eq(key, curr->key)){
            return curr->value;
        }
        curr = (HashEntry*)curr->next;
    }
    return NULL;
}
