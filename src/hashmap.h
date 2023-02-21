#ifndef HASHMAP_H
#define HASHMAP_H

#include "common.c"

#define INIT_CAPACITY 16

typedef struct
{
    const char* key;
    void* value;
    
    //struct pair* next;
} pair_t;

typedef struct
{
    pair_t* buckets;

    size_t capacity;
    size_t length;
} hashmap_t;

hashmap_t* hashmap_create(void);
void hashmap_delete(hashmap_t* map);

static uint64_t hash_key(const char* key);

void* hashmap_get(hashmap_t* map, const char* key);
const char* hashmap_set(hashmap_t* map, const char* key, void* value);

size_t hashmap_length(hashmap_t* map);

#endif // HASHMAP_H