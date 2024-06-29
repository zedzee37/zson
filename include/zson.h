#ifndef ZSON_H
#define ZSON_H

#include "smap.h"
#include "parser.h"
#include "allocators.h"

typedef struct {
    double n;
    char *s;
    bool b;
    StrHashMap *map;    
    ArenaAllocator *arena;
} JsonElement;

typedef struct {
    unsigned int current;
    JsonElement *head;
    Parser *parser;
    ArenaAllocator *arena;
} Deserializer;

extern JsonElement *json_element_init(ArenaAllocator *arena);

extern Deserializer *deserializer_init(Parser *p);
extern void deserializer_free(Deserializer *d);
extern JsonElement *deserialize(Deserializer *d);
extern JsonElement *deserialize_object(Deserializer *d);
extern JsonElement *deserialize_array(Deserializer *d);
extern JsonElement *deserialize_string(Deserializer *d);
extern JsonElement *deserialize_number(Deserializer *d);
extern JsonElement *deserialize_bool(Deserializer *d);
extern Token *get_token(Deserializer *d);

#endif
