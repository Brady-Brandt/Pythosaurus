#pragma once 


#define DEFAULT_ARRAY_LIST_SIZE 100

typedef struct{
   void* data;
   int size;
   int capacity;
} ArrayList;


#define array_list_create(list, type) \
do { \
    list.size = 0; \
    list.capacity = DEFAULT_ARRAY_LIST_SIZE; \
    list.data = malloc(sizeof(type) * list.capacity); \
} while(0) 



#define array_list_resize(list, type) \
do { \
    list.capacity *= 2; \
    list.data = realloc(list.data, sizeof(type) * list.capacity); \
} while(0) 

#define array_list_append(list, type, value) \
    do { \
        if(list.size == list.capacity){ \
            array_list_resize(list, value); \
        } \
        type* temp = (type*)list.data; \
        temp[list.size] = value; \
        list.size++; \
    } while(0)

#define array_list_get(list, type, index)((type*)list.data)[index]

#define array_list_delete(list) \
    do { \
        list.size = 0; \
        if(list.capacity != 0) free(list.data); \
        list.capacity = 0; \
    } while(0)


