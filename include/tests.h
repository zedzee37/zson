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

enum TestCode {
    T_SUCCESS = 0,
    T_FAILURE = 1,
};

struct Test {
    char *test_name;
    enum TestCode (*test)(int, char **);
};

struct TestStruct {
    char a[200];
};

extern int accumulate_tests(int argc, char **argv);
enum TestCode test_map_add(int argc, char **argv);
enum TestCode test_hash(int argc, char **argv);
enum TestCode test_alotta_stuff(int argc, char **argv);
enum TestCode test_large_struct(int argc, char **argv);
enum TestCode test_remove(int argc, char **argv);
enum TestCode test_open_file(int argc, char **argv);
enum TestCode test_parse(int argc, char **argv);
enum TestCode test_parse_string(int argc, char **argv);
enum TestCode test_parse_num(int argc, char **argv);
enum TestCode test_parse_bool(int argc, char **argv);
enum TestCode test_deserialize(int argc, char **argv);

#endif
