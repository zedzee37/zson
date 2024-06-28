#include "allocators.h"

ArenaAllocator *init_arena() {
    ArenaAllocator *arena = (ArenaAllocator *)malloc(sizeof(ArenaAllocator));
    if (arena == NULL) {
        return NULL;
    }

    arena->mem = malloc(sizeof(void *) * 10);
    if (arena->mem == NULL) {
        return NULL;
    }

    arena->size = 10;
    arena->p = 0;
    return arena;
}

void *arena_alloc(ArenaAllocator *arena, size_t size) {
    if (arena->p >= arena->size) {
        arena-> size *= 2;
        arena->mem = realloc(arena->mem, sizeof(void *) * arena->size);

        if (arena->mem == NULL) {
            return NULL;
        }
    }

    void *data = malloc(size);
    if (data == NULL) {
        return NULL;
    }

    arena->mem[arena->p++] = data;
    return data;
}

void arena_deinit(ArenaAllocator *arena) {
    for (int i = 0; i < arena->p; i++) {
        void *d = arena->mem[i];
        if (d == NULL) {
            continue;
        }

        free(d);
    }
    free(arena->mem);
    free(arena);
}
