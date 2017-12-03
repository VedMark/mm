#include <assert.h>
#include <stdio.h>

#include "include/tests.h"
#include "../include/err_codes.h"
#include "../include/mmemory.h"

void init_0Page5Size_WrongParams() {
    int ret_code = 0;

    ret_code = init_(0, 5);
    destroy_();

    assert(ret_code == EWRPAR);
    if(ret_code == EWRPAR)
        printf("%s: PASSED\n",__FUNCTION__);
}

void init_8Page8Size_Success() {
    int ret_code = 0;

    ret_code = init_(8, 8);
    destroy_();

    assert(ret_code == SUCCESS);
    if(ret_code == SUCCESS)
        printf("%s: PASSED\n",__FUNCTION__);
}

void init_32Page0x10000Size_Success() {
    int ret_code = 0;

    ret_code = init_(32, 0x100000);
    destroy_();

    assert(ret_code == SUCCESS);
    if(ret_code == SUCCESS)
        printf("%s: PASSED\n",__FUNCTION__);
}

void run_init_tests(void) {
    printf("UNIT TESTS: init_\n");

    init_0Page5Size_WrongParams();
    init_8Page8Size_Success();
    init_32Page0x10000Size_Success();

    printf("\n");
}
