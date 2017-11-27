#include "tests/test_init.h"
#include "tests/test_malloc.h"
#include "tests/test_free.h"
#include "tests/test_write.h"
#include "tests/test_read.h"

int main(int argc, int argv) {
    run_init_tests();
    run_malloc_tests();
    run_free_tests();
    run_write_tests();
    run_read_tests();

    return 0;
}
