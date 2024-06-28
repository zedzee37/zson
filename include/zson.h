#ifndef ZSON_H
#define ZSON_H

#include "smap.h"
#include "parser.h"

typedef struct {
    double n;
    char *s;
    bool b;
    StrHashMap *map;    
} JsonElement;

typedef struct {
    unsigned int c;
    JsonElement *head;
    Parser *p;
} Deserializer;

extern JsonElement *json_element_init();
extern void json_element_free(JsonElement *j);

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
