#include "smap.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int smap_hash(char *s) {
    unsigned long hash = 5381;
    int c;

    while (c = *s++)
        hash = ((hash << 5) + hash) + c;
    
    return hash;
}

StrHashMap *smap_init(StrHashMapCode *c, ArenaAllocator *allocator) {
    StrHashMap *map = arena_alloc(allocator, sizeof(StrHashMap));
    if (map == NULL) {
        perror("map here");
        *c = SMAP_FAILURE;
        return NULL;
    }

    map->data = arena_alloc(allocator, sizeof(void *) * 10);
    if (map->data == NULL) {
        perror("data init");
        *c = SMAP_FAILURE;
        return NULL;
    }

    map->item_count = 0;
    map->size = 10;
    *c = SMAP_SUCCESS;
    return map;
}

void smap_put(StrHashMap *m, char *k, void *v, StrHashMapCode *c) {
    if (m->item_count >= m->size) {
        m->data = realloc(m->data, sizeof(void *) * m->size * 2);
        if (m->data == NULL) {
            *c = SMAP_FAILURE;
            return;
        }
        m->size *= 2;
    }

    int h = smap_hash(k) % m->size;
    if (h >= m->size || h < 0) {
        *c = SMAP_FAILURE;
        return;
    }

    m->data[h] = v;
    *c = SMAP_SUCCESS;
}

void *smap_get(StrHashMap *m, char *k, StrHashMapCode *c) {
    int h = smap_hash(k) % m->size;

    if (h >= m->size || h < 0) {
        *c = SMAP_FAILURE;
        return NULL;
    }

    *c = SMAP_SUCCESS;
    return m->data[h];
}

void smap_remove(StrHashMap *m, char *k, StrHashMapCode *c) {
    int h = smap_hash(k) % m->size;

    if (h < 0 || h >= m->size) {
        *c = SMAP_FAILURE;
        return;
    }

    void *v = m->data[h];
    if (v == NULL) {
        *c = SMAP_FAILURE;
        return;
    }

    m->data[h] = NULL;
    m->item_count--; 
    *c = SMAP_SUCCESS;
}

