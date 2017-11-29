#ifndef MM_TEST_INIT_H
#define MM_TEST_INIT_H

#include <stdio.h>
#include <zconf.h>

#include "../include/err_codes.h"
#include "../include/mmemory.h"

void _init_test(int n, int szPage, int ret_code) {
    static u_int i = 0;
    printf("Test %d: ", ++i);
    if(init_(n, szPage) == ret_code) {
        printf("PASSED\n");
    }
    else {
        printf("FAILED\n");
    }
    destroy_();
}

void run_init_tests(void) {
    printf("UNIT TESTS: init_\n");

    _init_test(0, 5, EWRPAR);
    _init_test(8, 8, SUCCESS);
    _init_test(8, 0x8000, SUCCESS);
    _init_test(8, 0x10000000, EWRPAR);

    printf("\n");
}

#endif //MM_TEST_INIT_H
