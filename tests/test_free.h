#ifndef MM_TEST_FREE_H
#define MM_TEST_FREE_H

#include <stdlib.h>
#include <stdio.h>
#include <zconf.h>

#include "../include/err_codes.h"
#include "../include/mmemory.h"

void _free_test(VA ptr, int ret_code) {
    static u_int i = 0;
    printf("Test %d: ", ++i);
    if(free_(ptr) == ret_code) {
        printf("PASSED\n");
    }
    else {
        printf("FAILED\n");
    }
}

void run_free_tests(void) {
    printf("UNIT TESTS: free_\n");
    init_(64, 64);

    VA *ptr1 = malloc(sizeof(VA));
    VA *ptr2 = malloc(sizeof(VA));
    malloc_(ptr1, 32);
    malloc_(ptr2, 128);

    _free_test(*ptr1, SUCCESS);
    _free_test(*ptr2, SUCCESS);

    free(ptr1);
    free(ptr2);
    destroy_();
    printf("\n");
}

#endif //MM_TEST_FREE_H
