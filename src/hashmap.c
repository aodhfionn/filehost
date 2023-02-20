#include "hashmap.h"

hashmap_t* hashmap_create(void)
{
    hashmap_t* map = malloc(sizeof(hashmap_t));
    if (map == NULL) { return NULL; }

    map->length = 0;
    map->capacity = INIT_CAPACITY;

    // allocate buckets and make space for pairs
    map->buckets = calloc(map->capacity, sizeof(pair_t));
    if (map->buckets == NULL)
    {
        free(map);
        return NULL;
    }

    return map;
}

void hashmap_delete(hashmap_t* map)
{
    for (int i = 0; i < map->capacity; i++)
    {
        free((void*)map->buckets[i].key);
    }

    free(map->buckets);
    free(map);
}

// offset constants (unsigned long)
#define FNV_OFFSET 14695981039346656037UL
#define FNV_PRIME 1099511628211UL

// returns 64-bit FNV-1a hash for key (NUL-terminated)
// https://en.wikipedia.org/wiki/Fowler–Noll–Vo_hash_function
static uint64_t hash_key(const char* key)
{
    uint64_t hash = FNV_OFFSET;

    for (const char* p = key; *p; p++)
    {
        hash ^= (uint64_t)(unsigned char)(*p);
        hash *= FNV_PRIME;
    }

    return hash;
}

void* hashmap_get(hashmap_t* map, const char* key)
{
    uint64_t hash = hash_key(key);
    size_t index = (size_t)(hash & (uint64_t)(map->capacity - 1));

    while (map->buckets[index].key != NULL)
    {
        if (strcmp(key, map->buckets[index].key) == 0)
        {
            return map->buckets[index].value;
        }

        index++;
        if (index >= map->capacity) { index = 0; }
    }

    return NULL;
}

static const char* hashmap_set_bucket(pair_t* buckets, size_t capacity, const char* key, void* value, size_t* plength)
{
    uint64_t hash = hash_key(key);
    size_t index = (size_t)(hash & (uint64_t)(capacity - 1));

    while (buckets[index].key != NULL)
    {
        if (strcmp(key, buckets[index].key) == 0)
        {
            buckets[index].value = value;
            return buckets[index].key;
        }

        index++;
        if (index >= capacity) { index = 0; }
    }

    if (plength != NULL)
    {
        key = strdup(key);
        if (key == NULL) { return NULL; }

        (*plength)++;
    }

    buckets[index].key = (char*)key;
    buckets[index].value = value;

    return key;
}

static int hashmap_expand(hashmap_t* map)
{
    size_t new_cap = map->capacity * 2;
    if (new_cap < map->capacity) { return 0; }

    pair_t* new_buckets = calloc(new_cap, sizeof(pair_t));
    if (new_buckets == NULL) { return 0; }

    for (int i = 0; i < map->capacity; i++)
    {
        pair_t bucket = map->buckets[i];
        
        if (bucket.key != NULL)
        {
            hashmap_set_bucket(new_buckets, new_cap, bucket.key, bucket.value, NULL);
        }
    }

    free(map->buckets);
    map->buckets = new_buckets;
    map->capacity = new_cap;

    return 1;
}

const char* hashmap_set(hashmap_t* map, const char* key, void* value)
{
    if (value == NULL) { return NULL; }

    if (map->length >= map->capacity / 2)
    {
        if (!hashmap_expand(map)) { return NULL; }
    }

    return hashmap_set_bucket(map->buckets, map->capacity, key, value, &map->length);
}

size_t hashmap_length(hashmap_t* map)
{
    return map->length;
}