#pragma once 

#define DEFAULT_ARRAY_LIST_SIZE 100

typedef struct{
   void* data;
   int size;
   int capacity;
} ArrayList;

#define DEFAULT_ARRAY_LIST NULL


#define array_list_size(list) ((list == NULL) ? 0 : list->size)

#define array_list_create(list, type) \
do { \
    list = malloc(sizeof(ArrayList)); \
    if(list == NULL) break; \
    list->size = 0; \
    list->capacity = DEFAULT_ARRAY_LIST_SIZE; \
    list->data = malloc(sizeof(type) * list->capacity); \
} while(0) 


#define array_list_create_cap(list, type, cap) \
do { \
    list = malloc(sizeof(ArrayList)); \
    if(list == NULL) break; \
    list->size = 0; \
    list->capacity = cap; \
    list->data = malloc(sizeof(type) * list->capacity); \
} while(0) 


#define array_list_resize(list, type) \
do { \
    list->capacity *= 2; \
    list->data = realloc(list->data, sizeof(type) * list->capacity); \
} while(0) 

#define array_list_append(list, type, value) \
    do { \
        if(list->size == list->capacity){ \
            array_list_resize(list, type); \
        } \
        type* temp = (type*)list->data; \
        temp[list->size] = value; \
        list->size++; \
    } while(0)

#define array_list_get(list, type, index)((type*)list->data)[index]

#define array_list_delete(list) \
    do { \
        if(list != NULL) {  \
            if(list->capacity != 0) free(list->data); \
        } \
        free(list); \
    } while(0)


