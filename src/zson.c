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

JsonElement *json_element_init() {
    JsonElement *j = (JsonElement *)malloc(sizeof(JsonElement));
    if (j == NULL) {
        return NULL;
    }

    StrHashMapCode c;
    StrHashMap *map = smap_init(&c);
    if (c == SMAP_FAILURE) {
        perror("map");
        return NULL;
    }

    j->map = map;
    return j;
}

void json_element_free(JsonElement *j) {
    if (j == NULL) {
        return;
    }

    free(j);
}

Deserializer *deserializer_init(Parser *p) {
    Deserializer *d = (Deserializer *)malloc(sizeof(Deserializer));
    d->p = p;
    d->c = 0;
    return d;
}

void deserializer_free(Deserializer *d) {
    json_element_free(d->head);
    free(d);
}

JsonElement *deserialize(Deserializer *d) {
    JsonElement *j = deserialize_object(d);
    d->head = j;
    return j;
}

JsonElement *deserialize_object(Deserializer *d) {
    if (get_token(d)->type != BRACE_L) {
        return deserialize_array(d);
    }

    d->c++;
    Token *str;

    JsonElement *j = json_element_init();
    while (1) {
        switch (get_token(d)->type) {
            case STRING:
                str = get_token(d);
                d->c++;
                
                if (get_token(d)->type != COLON) {
                    perror("missing colon");
                }

                d->c++;
                
                StrHashMapCode c;
                JsonElement *j1 = deserialize_object(d);
                smap_put(j->map, str->s, j1, &c);

                if (c != SMAP_SUCCESS) {
                    perror("did not add");
                }

                Token *t = get_token(d);
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

JsonElement *deserialize_array(Deserializer *d) {
    if (get_token(d)->type != BRACKET_L) {
        return deserialize_string(d);
    }

    d->c++;

    JsonElement *j = json_element_init();
    int i = 0;
    while (1) {
        JsonElement *j1 = deserialize_object(d);
        
        Token *t = get_token(d);
        char n[50];
        sprintf(n, "%d", i);
        
        StrHashMapCode c;
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

JsonElement *deserialize_string(Deserializer *d) {
    if (get_token(d)->type != STRING) {
        return deserialize_number(d);
    }

    Token *t = get_token(d);
    d->c++;
    JsonElement *j = json_element_init();
    j->s = t->s;
    return j;
}

JsonElement *deserialize_number(Deserializer *d) {
    if (get_token(d)->type != NUMBER) {
        return deserialize_bool(d);
    }
    
    Token *t = get_token(d);
    d->c++;
    JsonElement *j = json_element_init();
    j->n = t->n;
    return j;
}

JsonElement *deserialize_bool(Deserializer *d) {
    if (get_token(d)->type != BOOL) {
        perror("did not find match");
        return NULL;
    }
    
    Token *t = get_token(d);
    d->c++;

    JsonElement *j = json_element_init();
    if (j == NULL) {  // Check if j is NULL
        perror("json_element_init failed");
        return NULL;
    }

    j->b = t->b;
    return j;
}

Token *get_token(Deserializer *d) {
    return &d->p->tokens[d->c];
}
