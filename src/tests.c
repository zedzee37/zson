#include "tests.h"
#include "allocators.h"
#include "parser.h"
#include "smap.h"
#include "zson.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int accumulate_tests(int argc, char **argv) {
    Test tests[] = {
        {"map add", test_map_add},
        {"hash", test_hash},
        {"large map", test_alotta_stuff},
        {"large struct", test_large_struct},
        {"remove", test_remove},
        {"open file", test_open_file},
        {"parse", test_parse},
        {"parse string", test_parse_string},
        {"parse num", test_parse_num},
        {"parse bool", test_parse_bool},
        {"deserialize", test_deserialize},
    };

    for (int i = 0; i < sizeof(tests) / sizeof(Test); i++) {
        Test t = tests[i];

        printf("\x1B[33mRunning test %s\n\x1B[37m", t.test_name);

        TestCode c = t.test(argc, argv);
        switch (c) {
            case T_SUCCESS:
                printf("\x1B[32mSucceeded.\n\n\x1B[37m");
                break;
            case T_FAILURE:
                printf("\x1B[31mFailed test: %s.\n\n\x1B[37m", t.test_name);
                return -1;
        }
    }

    return 0;
}

TestCode test_map_add(int argc, char **argv) {
    TestCode code = T_SUCCESS;

    StrHashMapCode c;
    ArenaAllocator *arena = init_arena();
    StrHashMap *m = smap_init(&c, arena);
    if (c == SMAP_FAILURE) {
        perror("malloc");
        code = T_FAILURE;
        goto exit;
    }

    int a = 10;
    smap_put(m, "guh", &a, &c);
    if (c == SMAP_FAILURE) {
        perror("add");
        code = T_FAILURE;
        goto exit;
    }

    int *b = smap_get(m, "guh", &c);
    if (c == SMAP_FAILURE) {
        perror("get");
        code = T_FAILURE;
        goto exit;
    }

    T_ASSERT(*b == a, code);

exit:
    arena_deinit(arena);

    return code;
}

TestCode test_hash(int argc, char **argv) {
    TestCode code = T_SUCCESS;

    int h1 = (smap_hash("guh") % 10);
    int h2 = smap_hash("guh") % 20;

    int h3 = smap_hash("gwefewfwfwfewf") % 10;
    int h4 = smap_hash("gwefewfwfwfewf") % 20;

    T_ASSERT(h1 == h2, code);
    T_ASSERT(h3 == h4, code);

exit:
    return T_SUCCESS;
}

TestCode test_alotta_stuff(int argc, char **argv) {
    TestCode code = T_SUCCESS;

    StrHashMapCode c;
    ArenaAllocator *arena = init_arena();
    StrHashMap *m = smap_init(&c, arena);
    if (c == SMAP_FAILURE) {
        perror("malloc");
        code = T_FAILURE;
        goto exit;
    }

    char buf[21];
    buf[20] = '\0';
    memset(buf, '0', 20);
    for (int i = 0; i < 20; i++) {
        buf[i] = 'a';
        smap_put(m, buf, &i, &c);
        if (c == SMAP_FAILURE) {
            perror("Could not add");
            code = T_FAILURE;
            goto exit;
        }
    }

    int *val = (int *)smap_get(m, buf, &c);
    if (c == SMAP_FAILURE) {
        perror("could not get");
        code = T_FAILURE;
        goto exit;
    }

    T_ASSERT(*val == 20, code);

exit:
    arena_deinit(arena);

    return code;
}

TestCode test_large_struct(int argc, char **argv) {
    TestCode code = T_SUCCESS;

    StrHashMapCode c;
    ArenaAllocator *arena = init_arena();
    StrHashMap *m = smap_init(&c, arena);
    if (c == SMAP_FAILURE) {
        perror("malloc");
        code = T_FAILURE;
        goto exit;
    }
    struct TestStruct s;
    memset(s.a, '0', 199);
    s.a[199] = '\0';
    smap_put(m, "guh", &s, &c);

    if (c == SMAP_FAILURE) {
        perror("add");
        code = T_FAILURE;
        goto exit;
    }

    struct TestStruct *g = (struct TestStruct *)smap_get(m, "guh", &c);
    if (c == SMAP_FAILURE) {
        perror("get");
        code = T_FAILURE;
        goto exit;
    }

    char *a = s.a;
    char *b = g->a;

    T_ASSERT(a == b, code);

exit:
    arena_deinit(arena);

    return code;
}

TestCode test_remove(int argc, char **argv) {
    TestCode code = T_SUCCESS;

    StrHashMapCode c;
    ArenaAllocator *arena = init_arena();
    StrHashMap *m = smap_init(&c, arena);
    if (c == SMAP_FAILURE) {
        perror("malloc");
        code = T_FAILURE;
        goto exit;
    }

    int a = 10;
    smap_put(m, "guh", &a, &c);
    if (c == SMAP_FAILURE) {
        perror("add");
        code = T_FAILURE;
        goto exit;
    }

    smap_remove(m, "guh", &c);
    if (c == SMAP_FAILURE) {
        perror("remove");
        code = T_FAILURE;
        goto exit;
    }

    int *v = (int *)smap_get(m, "guh", &c);

    T_ASSERT(v == NULL, code);

exit:
    arena_deinit(arena);
    return code;
}

TestCode test_open_file(int argc, char **argv) {
    TestCode code = T_SUCCESS;

    Parser *p = parser_init();
    switch (parser_read_file(p, argv[1])) {
        case PASS:
            break;
        case FAILURE:
            code = T_FAILURE;
            goto exit;
    }

    T_ASSERT(p->file[0] == '{', code);

exit:
    free(p->file);
    free(p);
    free(p->tokens);

    return code;
}

TestCode test_parse(int argc, char **argv) {
    TestCode code = T_SUCCESS;

    Parser *p = parser_init();
    switch (parser_read_file(p, argv[1])) {
        case PASS:
            break;
        case FAILURE:
            code = T_FAILURE;
            goto exit;
    }

    ParserCode c = parser_parse(p);
    T_ASSERT(c == PASS, code);

    int i = 0;
    while (p->tokens[i].type != EOF) {
        Token t = p->tokens[i++];

        if (t.type == NUMBER) {
            printf("%f\n", t.n);
        }
    }

exit:
    parser_free(p);

    return code;
}

TestCode test_parse_string(int argc, char **argv) {
    TestCode code = T_SUCCESS;

    Parser *p = parser_init();
    p->file = "\"gorking\"";

    p->p++;
    char *c = match_string(p);

    char *c2 = "gorking";
    for (int i = 0; i < 7; i++) {
        T_ASSERT(c[i] == c2[i], code);
    }

exit:
    free(c);
    free(p->tokens);
    free(p);

    return code;
}

TestCode test_parse_num(int argc, char **argv) {
    TestCode code = T_SUCCESS;

    Parser *p = parser_init();
    p->file = "\"10.32151\"";

    double n;
    p->p++;
    match_num(p, p->file[p->p++], &n);

    T_ASSERT(n == 10.32151, code);

exit:
    free(p->tokens);
    free(p);

    return code;
}

TestCode test_parse_bool(int argc, char **argv) {
    TestCode code = T_SUCCESS;

    Parser *p = parser_init();
    p->file = "true";

    bool b;
    T_ASSERT(match_bool(p, p->file[p->p++], &b), code);

    T_ASSERT(b == true, code);

exit:
    free(p->tokens);
    free(p);

    return code;
}

TestCode test_deserialize(int argc, char **argv) { 
    TestCode code = T_SUCCESS;

    Parser *p = parser_init();
    switch (parser_read_file(p, argv[1])) {
        case PASS:
            break;
        case FAILURE:
            code = T_FAILURE;
            goto exit;
    }

    ParserCode c = parser_parse(p);
    T_ASSERT(c == PASS, code);

    Deserializer *d = deserializer_init(p);
    JsonElement *j = deserialize(d);

exit:
    parser_free(p);
    deserializer_free(d);

    return code;
}

