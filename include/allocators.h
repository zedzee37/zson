#ifndef ALLOCATORS_H
#define ALLOCATORS_H

#include <stdlib.h>

typedef struct {
    void **mem;
    size_t size; 
    size_t p;
} ArenaAllocator;

extern ArenaAllocator *init_arena();
extern void *arena_alloc(ArenaAllocator *arena, size_t size);
extern void arena_deinit(ArenaAllocator *arena);

#endif
