#include "tests/include/tests.h"

void __assert_fail(const char * assertion, const char * file, unsigned int line, const char * function) {
    fprintf(stdout, "%s: FAILED: assertion `%s` at %s:%d \n", function, assertion, file, line);

}

int main(int argc, char *argv[]) {
    run_init_tests();
    run_free_tests();
    run_malloc_tests();
    run_write_tests();
    run_read_tests();

    run_load_test();

    return 0;
}
