#include "smap.h"
#include <stdlib.h>
#include <string.h>

int smap_hash(char *s) {
    unsigned long hash = 5381;
    int c;

    while (c = *s++)
        hash = ((hash << 5) + hash) + c;
    
    return hash;
}

struct StrHashMap *smap_init(enum StrHashMapCode *c) {
    struct StrHashMap *map = (struct StrHashMap *)malloc(sizeof(struct StrHashMap));
    if (map == NULL) {
        *c = SMAP_COULD_NOT_CREATE;
        return NULL;
    }

    map->data = malloc(sizeof(void *) * 10);
    if (map->data == NULL) {
        *c = SMAP_COULD_NOT_CREATE;
        return NULL;
    }

    map->item_count = 0;
    map->size = 10;
    *c = SMAP_SUCCESS;
    return map;
}

void smap_put(struct StrHashMap *m, char *k, void *v, enum StrHashMapCode *c) {
    if (m->item_count >= m->size) {
        m->data = realloc(m->data, sizeof(void *) * m->size * 2);
        if (m->data == NULL) {
            *c = SMAP_COULD_NOT_ADD;
            return;
        }
        m->size *= 2;
    }

    int h = smap_hash(k) % m->size;
    if (h >= m->size || h < 0) {
        *c = SMAP_COULD_NOT_ADD;
        return;
    }

    m->data[h] = v;
    *c = SMAP_SUCCESS;
}

void *smap_get(struct StrHashMap *m, char *k, enum StrHashMapCode *c) {
    int h = smap_hash(k) % m->size;

    if (h >= m->size || h < 0) {
        *c = SMAP_COULD_NOT_GET;
        return NULL;
    }

    *c = SMAP_SUCCESS;
    return m->data[h];
}

void smap_remove(struct StrHashMap *m, char *k, enum StrHashMapCode *c) {
    int h = smap_hash(k) % m->size;

    if (h < 0 || h >= m->size) {
        *c = SMAP_COULD_NOT_REMOVE;
        return;
    }

    void *v = m->data[h];
    if (v == NULL) {
        *c = SMAP_COULD_NOT_REMOVE;
        return;
    }

    m->data[h] = NULL;
    m->item_count--; 
    *c = SMAP_SUCCESS;
}

void smap_free(struct StrHashMap *m) {
    free(m->data);
    free(m);
}
