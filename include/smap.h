#ifndef SMAP_H
#define SMAP_H

#include <stddef.h>

typedef enum {
    SMAP_SUCCESS = 1,
    SMAP_FAILURE = 0,
} StrHashMapCode;

typedef struct {
    void **data;
    size_t size;
    size_t item_count;
} StrHashMap;

extern int smap_hash(char *s);
extern StrHashMap *smap_init(StrHashMapCode *c);
extern void smap_put(StrHashMap *m, char *k, void *v, StrHashMapCode *c);
extern void *smap_get(StrHashMap *m, char *k, StrHashMapCode *c);
extern void smap_remove(StrHashMap *m, char *k, StrHashMapCode *c);
extern void smap_free(StrHashMap *m);

#endif
