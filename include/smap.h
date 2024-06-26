#ifndef SMAP_H
#define SMAP_H

#include <stddef.h>

enum StrHashMapCode {
    SMAP_SUCCESS = 0,
    SMAP_COULD_NOT_CREATE = 1,
    SMAP_COULD_NOT_ADD = 2,
    SMAP_COULD_NOT_REMOVE = 3,
    SMAP_COULD_NOT_GET = 4,
};

struct StrHashMap {
    void **data;
    size_t size;
    size_t item_count;
};

extern int smap_hash(char *s);
extern struct StrHashMap *smap_init(enum StrHashMapCode *c);
extern void smap_put(struct StrHashMap *m, char *k, void *v, enum StrHashMapCode *c);
extern void *smap_get(struct StrHashMap *m, char *k, enum StrHashMapCode *c);
extern void smap_remove(struct StrHashMap *m, char *k, enum StrHashMapCode *c);
extern void smap_free(struct StrHashMap *m);

#endif
