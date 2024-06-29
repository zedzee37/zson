#include "zson.h"
#include "allocators.h"
#include "parser.h"
#include "tests.h"
#include "smap.h"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
#ifdef DEBUGF
    return accumulate_tests(argc, argv);
#endif
    int code = 0;

    if (argc < 2) {
        perror("Missing file argument.");
        return -1;
    }

    Parser *p = parser_init();

    if (p == NULL) {
        perror("Could not allocate memory for Parser");
        code = -1;
        goto parser_exit;
    }

    switch (parser_read_file(p, argv[1])) {
        case PASS:
            break;
        case FAILURE:
            code = -1;
            goto parser_exit;
    }

    ParserCode c = parser_parse(p);

    Deserializer *d = deserializer_init(p);
    if (d == NULL) {
        perror("Could not allocate memory for Deserializer.");
        code = -1;
        goto exit;
    }

    StrHashMapCode i;
    JsonElement *j = deserialize(d);
    printf("%f\n", ((JsonElement *)smap_get(j->map, "guh", &i))->n);

exit:
    deserializer_free(d);
parser_exit:
    parser_free(p);

leave:
    return code;
}

JsonElement *json_element_init(ArenaAllocator *arena) {
    JsonElement *j = arena_alloc(arena, sizeof(JsonElement));
    if (j == NULL) {
        return NULL;
    }

    StrHashMapCode c;
    StrHashMap *map = smap_init(&c, arena);
    if (c == SMAP_FAILURE) {
        perror("map");
        return NULL;
    }

    j->map = map;
    return j;
}

Deserializer *deserializer_init(Parser *p) {
    Deserializer *d = (Deserializer *)malloc(sizeof(Deserializer));
    if (d == NULL) {
        return NULL;
    }

    ArenaAllocator *allocator = init_arena();
    d->parser = p;
    d->current = 0;
    d->arena = allocator;
    return d;
}

void deserializer_free(Deserializer *d) {
    arena_deinit(d->arena);
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

    d->current++;
    Token *str;

    JsonElement *j = json_element_init(d->arena);
    while (1) {
        switch (get_token(d)->type) {
            case STRING:
                str = get_token(d);
                d->current++;
                
                if (get_token(d)->type != COLON) {
                    perror("missing colon");
                }

                d->current++;
                
                StrHashMapCode c;
                JsonElement *j1 = deserialize_object(d);
                smap_put(j->map, str->s, j1, &c);

                if (c != SMAP_SUCCESS) {
                    perror("did not add");
                }

                Token *t = get_token(d);
                if (t->type == COMMA) {
                    d->current++;
                    break;
                }

                if (t->type != BRACE_R) {
                    perror("Missing end brace.");
                    return NULL;
                }

                d->current++;

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

    d->current++;

    JsonElement *j = json_element_init(d->arena);
    int i = 0;
    while (1) {
        JsonElement *j1 = deserialize_object(d);
        
        Token *t = get_token(d);
        char n[50];
        sprintf(n, "%d", i);
        
        StrHashMapCode c;
        smap_put(j->map, n, j1, &c);

        if (t->type == COMMA) {
            d->current++;
            i++;
            continue;
        }

        if (t->type != BRACKET_R) {
            perror("Missing end bracket.");
            return NULL;
        }

        d->current++;
        return j;
    }
}

JsonElement *deserialize_string(Deserializer *d) {
    if (get_token(d)->type != STRING) {
        return deserialize_number(d);
    }

    Token *t = get_token(d);
    d->current++;
    JsonElement *j = json_element_init(d->arena);
    j->s = t->s;
    return j;
}

JsonElement *deserialize_number(Deserializer *d) {
    if (get_token(d)->type != NUMBER) {
        return deserialize_bool(d);
    }
    
    Token *t = get_token(d);
    d->current++;
    JsonElement *j = json_element_init(d->arena);
    j->n = t->n;
    return j;
}

JsonElement *deserialize_bool(Deserializer *d) {
    if (get_token(d)->type != BOOL) {
        perror("did not find match");
        return NULL;
    }
    
    Token *t = get_token(d);
    d->current++;

    JsonElement *j = json_element_init(d->arena);
    if (j == NULL) {  // Check if j is NULL
        perror("json_element_init failed");
        return NULL;
    }

    j->b = t->b;
    return j;
}

Token *get_token(Deserializer *d) {
    return &d->parser->tokens[d->current];
}
