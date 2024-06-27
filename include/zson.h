#ifndef ZSON_H
#define ZSON_H

#include "smap.h"
#include "parser.h"

struct JsonElement {
    double n;
    char *s;
    bool b;
    struct StrHashMap *map;    
};

struct Deserializer {
    unsigned int c;
    struct JsonElement *head;
    struct Parser *p;
};

extern struct JsonElement *json_element_init();
extern void json_element_free(struct JsonElement *j);

extern struct Deserializer *deserializer_init(struct Parser *p);
extern void deserializer_free(struct Deserializer *d);
extern struct JsonElement *deserialize(struct Deserializer *d);
extern struct JsonElement *deserialize_object(struct Deserializer *d);
extern struct JsonElement *deserialize_array(struct Deserializer *d);
extern struct JsonElement *deserialize_string(struct Deserializer *d);
extern struct JsonElement *deserialize_number(struct Deserializer *d);
extern struct JsonElement *deserialize_bool(struct Deserializer *d);
extern struct Token *get_token(struct Deserializer *d);

#endif
