#include "zson.h"
#include "tests.h"
#include "smap.h"

int main(int argc, char *argv[]) {
#ifdef DEBUGF
    return accumulate_tests(argc, argv);
#endif
    return 0;
}
