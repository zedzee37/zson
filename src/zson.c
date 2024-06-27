#include "zson.h"
#include "parser.h"
#include "tests.h"
#include "smap.h"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
#ifdef DEBUGF
    return accumulate_tests(argc, argv);
#endif
    return 0;
}

struct JsonElement *json_element_init() {
    struct JsonElement *j = (struct JsonElement *)malloc(sizeof(struct JsonElement));
    if (j == NULL) {
        return NULL;
    }

    enum StrHashMapCode c;
    struct StrHashMap *map = smap_init(&c);
    if (c == SMAP_COULD_NOT_CREATE) {
        perror("map");
        return NULL;
    }

    j->map = map;
    return j;
}

void json_element_free(struct JsonElement *j) {
    if (j == NULL) {
        return;
    }

    free(j);
}

struct Deserializer *deserializer_init(struct Parser *p) {
    struct Deserializer *d = (struct Deserializer *)malloc(sizeof(struct Deserializer));
    d->p = p;
    d->c = 0;
    return d;
}

void deserializer_free(struct Deserializer *d) {
    json_element_free(d->head);
    free(d);
}

struct JsonElement *deserialize(struct Deserializer *d) {
    struct JsonElement *j = deserialize_object(d);
    d->head = j;
    return j;
}

struct JsonElement *deserialize_object(struct Deserializer *d) {
    if (get_token(d)->type != BRACE_L) {
        return deserialize_array(d);
    }

    d->c++;
    struct Token *str;

    struct JsonElement *j = json_element_init();
    while (1) {
        switch (get_token(d)->type) {
            case STRING:
                str = get_token(d);
                d->c++;
                
                if (get_token(d)->type != COLON) {
                    perror("missing colon");
                }

                d->c++;
                
                enum StrHashMapCode c;
                struct JsonElement *j1 = deserialize_object(d);
                smap_put(j->map, str->s, j1, &c);

                if (c != SMAP_SUCCESS) {
                    perror("did not add");
                }

                struct Token *t = get_token(d);
                if (t->type == COMMA) {
                    d->c++;
                    break;
                }

                if (t->type != BRACE_R) {
                    perror("Missing end brace.");
                    return NULL;
                }

                return j;
                break;
            case BRACE_R:
                return j;
                break;
            default:
                perror("Syntax error\n");
                return NULL;
                break;
        }
    }
}

struct JsonElement *deserialize_array(struct Deserializer *d) {
    if (get_token(d)->type != BRACKET_L) {
        return deserialize_string(d);
    }

    d->c++;

    struct JsonElement *j = json_element_init();
    int i = 0;
    while (1) {
        struct JsonElement *j1 = deserialize_object(d);
        
        struct Token *t = get_token(d);
        char n[50];
        sprintf(n, "%d", i);
        
        enum StrHashMapCode c;
        smap_put(j->map, n, j1, &c);

        if (t->type == COMMA) {
            d->c++;
            i++;
            continue;
        }

        if (t->type != BRACKET_R) {
            perror("Missing end bracket.");
            return NULL;
        }

        d->c++;
        return j;
    }
}

struct JsonElement *deserialize_string(struct Deserializer *d) {
    if (get_token(d)->type != STRING) {
        return deserialize_number(d);
    }

    struct Token *t = get_token(d);
    d->c++;
    struct JsonElement *j = json_element_init();
    j->s = t->s;
    return j;}

struct JsonElement *deserialize_number(struct Deserializer *d) {
    if (get_token(d)->type != NUMBER) {
        return deserialize_bool(d);
    }
    
    struct Token *t = get_token(d);
    d->c++;
    struct JsonElement *j = json_element_init();
    j->n = t->n;
    return j;
}

struct JsonElement *deserialize_bool(struct Deserializer *d) {
    if (get_token(d)->type != BOOL) {
        perror("did not find match");
        return NULL;
    }
    
    struct Token *t = get_token(d);
    d->c++;

    struct JsonElement *j = json_element_init();
    if (j == NULL) {  // Check if j is NULL
        perror("json_element_init failed");
        return NULL;
    }

    j->b = t->b;
    return j;
}

struct Token *get_token(struct Deserializer *d) {
    return &d->p->tokens[d->c];
}
