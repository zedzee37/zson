#ifndef TESTS_H
#define TESTS_H

#define T_ASSERT(b, c)                                                         \
    do {                                                                         \
        char *s = #b;                                                              \
        if (!(b)) {                                                                \
            printf(" | Assertion %s did not succeed.\n", s);                         \
            (c) = T_FAILURE;                                                         \
            goto exit;                                                               \
        } else {                                                                   \
            printf(" | Assertion %s succeeded.\n", s);                               \
        }                                                                          \
    } while (0)

typedef enum {
    T_SUCCESS = 0,
    T_FAILURE = 1,
} TestCode;

typedef struct {
    char *test_name;
    TestCode (*test)(int, char **);
} Test;

struct TestStruct {
    char a[200];
};

extern int accumulate_tests(int argc, char **argv);
TestCode test_map_add(int argc, char **argv);
TestCode test_hash(int argc, char **argv);
TestCode test_alotta_stuff(int argc, char **argv);
TestCode test_large_struct(int argc, char **argv);
TestCode test_remove(int argc, char **argv);
TestCode test_open_file(int argc, char **argv);
TestCode test_parse(int argc, char **argv);
TestCode test_parse_string(int argc, char **argv);
TestCode test_parse_num(int argc, char **argv);
TestCode test_parse_bool(int argc, char **argv);
TestCode test_deserialize(int argc, char **argv);

#endif
