#include <malloc.h>
#include <time.h>
#include <zconf.h>

#include "include/tests.h"
#include "../include/mmemory.h"

void fill_buffer(char *pBuffer, size_t szBuffer) {
    for(u_int i = 0; i < szBuffer; ++i) {
        *(pBuffer + i) = 'a';
    }
}

void print_array(int *arr, size_t szArr) {
    printf("[");
    for(u_int i = 0; i < szArr; ++i) {
        printf("%d,", arr[i]);
    }
    printf("]\n");

}

void run_load_test(void) {

    time_t tBegin = 0;
    time_t tEnd = 0;
    u_int k = 0;
    VA *ptr = NULL;
    char *pwBuffer = NULL;
    char *prBuffer = NULL;

    printf("n-dependensy\n");

    int *x = malloc((0x3000 / 500 + 1) * sizeof(int));
    int *y = malloc((0x3000 / 500 + 1) * sizeof(int));

    pwBuffer = malloc(0x2000);
    prBuffer = malloc(0x2000);
    fill_buffer(pwBuffer, 0x2000);
    for(u_int i = 10; i < 0x3000; i += 500) {
        ptr = malloc((i / 2 - 1) * sizeof(VA));
        tBegin = clock();

        init_(i, 0x1000);
        for(u_int j = 0; j < i / 2 - 1; ++j) {
            malloc_(&ptr[j], 0x2000);
        }
        for(u_int j = 0; j < i / 2 - 1; ++j) {
            write_(ptr[j], pwBuffer, 0x2000);
        }
        for(u_int j = 0; j < i / 2 - 1; ++j) {
            read_(ptr[j], prBuffer, 0x2000);
        }
        for(u_int j = 0; j < i / 2 - 1; ++j) {
            free_(ptr[j]);
        }
        destroy_();

        tEnd = clock();
        x[k] = i;
        y[k++] = (int) (tEnd - tBegin);
        free(ptr);
    }
    free(pwBuffer);
    free(prBuffer);

    print_array(x, k);
    print_array(y, k);
    printf("\nszPage-dependency\n");

    k = 0;
    ptr = malloc((0x10 - 1) * sizeof(VA));
    for(u_int i = 1; i < 0x2000000; i <<= 1) {
        pwBuffer = malloc(2 * i);
        prBuffer = malloc(2 * i);
        fill_buffer(pwBuffer, 2 * i);
        tBegin = clock();

        init_(0x20, i);
        for(u_int j = 0; j < 0x10 - 1; ++j) {
            malloc_(&ptr[j], 2 * i);
        }
        for(u_int j = 0; j < 0x10 - 1; ++j) {
            write_(ptr[j], pwBuffer, 2 * i);
        }
        for(u_int j = 0; j < 0x10 - 1; ++j) {
            read_(ptr[j], prBuffer, 2 * i);
        }
        for(u_int j = 0; j < 0x10 - 1; ++j) {
            free_(ptr[j]);
        }
        destroy_();

        tEnd = clock();
        x[k] = i;
        y[k++] = (int) (tEnd - tBegin);
        free(pwBuffer);
        free(prBuffer);
    }

    print_array(x, k);
    print_array(y, k);

    free(ptr);
    free(x);
    free(y);
    printf("\n");
}
