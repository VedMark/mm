#ifndef MM_TEST_WRITE_H
#define MM_TEST_WRITE_H

#include <stdlib.h>
#include <stdio.h>
#include <zconf.h>
#include <memory.h>

#include "../include/err_codes.h"
#include "../include/mmemory.h"

void _write_test(VA ptr, void *pBuffer, size_t szBuffer, int ret_code) {
    static u_int i = 0;
    printf("Test %d: ", ++i);
    if(write_(ptr, pBuffer, szBuffer) == ret_code) {
        printf("PASSED\n");
    }
    else {
        printf("FAILED\n");
    }
}

void run_write_tests(void) {
    printf("UNIT TESTS: write_\n");
    init_(4, 0x8000);

    VA ptr0 = (VA) 1;
    VA *ptr1 = malloc(sizeof(VA));
    VA *ptr2 = malloc(sizeof(VA));
    VA *ptr3 = malloc(sizeof(VA));
    VA *ptr4 = malloc(sizeof(VA));
    malloc_(ptr1, 32);
    malloc_(ptr2, 514);
    malloc_(ptr3, 0x8000);
    malloc_(ptr4, 0x8000);

    char *pBuffer1 = "Stranger things";
    char *pBuffer2 = "Fantastic Beasts and Where to Find Them";
    int arr[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
                 11,12,13,14,15,16,17,18,19,20,
                 21,22,23,24,25,26,27,28,29,30,
                 31,32,33,34,35,36,37,38,39,40,
                 41,42,43,44,45,46,47,48,49,50,
                 51,52,53,54,55,56,57,58,59,60,
                 61,62,63,64,65,66,67,68,69,70,
                 71,72,73,74,75,76,77,78,79,80,
                 81,82,83,84,85,86,87,88,89,90,
                 91,92,93,94,95,96,97,98,99,100};
    int arr1[] = {111};

    _write_test(NULL, pBuffer1, strlen(pBuffer1), EWRPAR);
    _write_test(ptr0, pBuffer1, strlen(pBuffer1), EWRPAR);
    _write_test(*ptr1 + 30, pBuffer1, strlen(pBuffer1), EOBORD);
    _write_test(*ptr1, pBuffer1, strlen(pBuffer1), SUCCESS);
    _write_test(*ptr1, pBuffer1, strlen(pBuffer1), SUCCESS);
    _write_test(*ptr2, pBuffer2, strlen(pBuffer2), SUCCESS);
    _write_test(*ptr4, arr, 100 * sizeof(int), SUCCESS);
    _write_test(*ptr4 + 10 * sizeof(int), arr1, 1 * sizeof(int), SUCCESS);

    free(ptr1);
    free(ptr2);
    free(ptr3);
    free(ptr4);

    destroy_();
    printf("\n");
}

#endif //MM_TEST_WRITE_H
