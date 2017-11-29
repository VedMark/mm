#ifndef MM_TEST_MALLOC_H
#define MM_TEST_MALLOC_H

#include <stdlib.h>
#include <stdio.h>
#include <zconf.h>

#include "../include/err_codes.h"
#include "../include/mmemory.h"

void _malloc_test(VA *ptr, size_t szBlock, int ret_code) {
    static u_int i = 0;
    printf("Test %d: ", ++i);
    if(malloc_(ptr, szBlock) == ret_code) {
        printf("PASSED\n");
    }
    else {
        printf("FAILED\n");
    }
}

void run_malloc_tests(void) {
    printf("UNIT TESTS: malloc_\n");
    init_(64, 64);

    VA *ptr = malloc(sizeof(VA));
    _malloc_test(ptr, 0, EWRPAR);
    _malloc_test(ptr, 32, SUCCESS);
    _malloc_test(ptr, 64, SUCCESS);
    _malloc_test(ptr, 32, SUCCESS);
    _malloc_test(ptr, 144, SUCCESS);
    _malloc_test(ptr, 32, SUCCESS);
    _malloc_test(ptr, 10, SUCCESS);
    _malloc_test(ptr, 128, SUCCESS);
    free_(*ptr);
    _malloc_test(ptr, 128, SUCCESS);
    _malloc_test(ptr, 128 * 128, EMLACK);

    free(ptr);
    destroy_();
    printf("\n");
}

#endif //MM_TEST_MALLOC_H
