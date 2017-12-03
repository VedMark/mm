#ifndef MM_TESTS_H
#define MM_TESTS_H

#include <zconf.h>
#include <stdio.h>

#define FILL_BUFFER(pBuffer, szBuffer) {        \
    for(u_int i = 0; i < (szBuffer) - 1; ++i) { \
        *((pBuffer) + i) = 'a';                 \
    }                                           \
    *((pBuffer) + (szBuffer) - 1) = '\0';         \
};

void run_init_tests(void);
void run_free_tests(void);
void run_malloc_tests(void);
void run_write_tests(void);
void run_read_tests(void);

void run_load_test(void);

#endif //MM_TESTS_H
