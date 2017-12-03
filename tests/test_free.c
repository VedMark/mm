#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "include/tests.h"
#include "../include/err_codes.h"
#include "../include/mmemory.h"

free_NullPtr_WrongParams(){
    int ret_code = 0;
    VA *ptr = malloc(sizeof(VA));
    *ptr = 0;
    init_(64, 64);

    ret_code = free_(*ptr);

    destroy_();
    free(ptr);
    assert(ret_code == EWRPAR);
    if(ret_code == EWRPAR)
        printf("%s: PASSED\n",__FUNCTION__);
}

void free_PtrFromNotAlocated_ErrUnknown() {
    int ret_code = 0;
    VA *ptr = malloc(sizeof(VA));
    init_(64, 64);
    *ptr = (VA) 0x60;

    ret_code = free_(*ptr);

    destroy_();
    free(ptr);
    assert(ret_code == EUNKNW);
    if(ret_code == EUNKNW)
        printf("%s: PASSED\n",__FUNCTION__);
}

void free_PtrFromBlockMiddle_ErrUnknown(){
    int ret_code = 0;
    VA *ptr = malloc(sizeof(VA));
    init_(64, 64);
    malloc_(ptr, 10);

    ret_code = free_(*ptr + 2);

    destroy_();
    free(ptr);
    assert(ret_code == EUNKNW);
    if(ret_code == EUNKNW)
        printf("%s: PASSED\n",__FUNCTION__);
}

void free_ptrMorePageSizeBlockWithNoPrev_Success(){
    int ret_code = 0;
    VA *ptr = malloc(sizeof(VA));
    init_(64, 64);
    malloc_(ptr, 120);

    ret_code = free_(*ptr);

    destroy_();
    free(ptr);
    assert(ret_code == SUCCESS);
    if(ret_code == SUCCESS)
        printf("%s: PASSED\n",__FUNCTION__);
}

void free_ptrMorePageSizeBlockWithPrevAloc_Success(){
    int ret_code = 0;
    VA *ptr = malloc(sizeof(VA));
    init_(64, 64);
    malloc_(ptr, 10);
    malloc_(ptr, 100);

    ret_code = free_(*ptr);

    destroy_();
    free(ptr);
    assert(ret_code == SUCCESS);
    if(ret_code == SUCCESS)
        printf("%s: PASSED\n",__FUNCTION__);
}


void free_ptrMorePageSizeBlockWithPrevFreeAndNextAloc_Success(){
    int ret_code = 0;
    VA *ptr1 = malloc(sizeof(VA));
    VA *ptr2 = malloc(sizeof(VA));
    VA *ptr3 = malloc(sizeof(VA));
    init_(64, 64);
    malloc_(ptr1, 10);
    malloc_(ptr2, 10);
    malloc_(ptr3, 100);
    free_(*ptr1);

    ret_code = free_(*ptr2);

    destroy_();
    free(ptr1);
    free(ptr2);
    free(ptr3);
    assert(ret_code == SUCCESS);
    if(ret_code == SUCCESS)
        printf("%s: PASSED\n",__FUNCTION__);
}

void free_ptrMorePageSizeBlockWithPrevFree_Success(){
    int ret_code = 0;
    VA *ptr1 = malloc(sizeof(VA));
    VA *ptr2 = malloc(sizeof(VA));
    init_(64, 64);
    malloc_(ptr1, 10);
    malloc_(ptr1, 10);
    malloc_(ptr2, 100);
    free_(*ptr1);

    ret_code = free_(*ptr2);

    destroy_();
    free(ptr1);
    free(ptr2);
    assert(ret_code == SUCCESS);
    if(ret_code == SUCCESS)
        printf("%s: PASSED\n",__FUNCTION__);
}

void free_ptrLessPageSizeNoPrev_Success(){
    int ret_code = 0;
    VA *ptr = malloc(sizeof(VA));
    init_(64, 64);
    malloc_(ptr, 8);

    ret_code = free_(*ptr);

    destroy_();
    free(ptr);
    assert(ret_code == SUCCESS);
    if(ret_code == SUCCESS)
        printf("%s: PASSED\n",__FUNCTION__);
}


void free_ptrLessPageSizeNoPrevNextAloc_Success(){
    int ret_code = 0;
    VA *ptr1 = malloc(sizeof(VA));
    VA *ptr2 = malloc(sizeof(VA));
    init_(64, 64);
    malloc_(ptr1, 8);
    malloc_(ptr1, 8);
    malloc_(ptr2, 8);

    ret_code = free_(*ptr1);

    destroy_();
    free(ptr1);
    free(ptr2);
    assert(ret_code == SUCCESS);
    if(ret_code == SUCCESS)
        printf("%s: PASSED\n",__FUNCTION__);
}


void free_ptrLessPageSizeWithPrevAloc_Success(){
    int ret_code = 0;
    VA *ptr = malloc(sizeof(VA));
    init_(64, 64);
    malloc_(ptr, 8);
    malloc_(ptr, 8);

    ret_code = free_(*ptr);

    destroy_();
    free(ptr);
    assert(ret_code == SUCCESS);
    if(ret_code == SUCCESS)
        printf("%s: PASSED\n",__FUNCTION__);
}

void run_free_tests(void) {
    printf("UNIT TESTS: free_\n");

    free_NullPtr_WrongParams();
    free_PtrFromNotAlocated_ErrUnknown();
    free_PtrFromBlockMiddle_ErrUnknown();
    free_ptrMorePageSizeBlockWithNoPrev_Success();
    free_ptrMorePageSizeBlockWithPrevAloc_Success();
    free_ptrMorePageSizeBlockWithPrevFreeAndNextAloc_Success();
    free_ptrMorePageSizeBlockWithPrevFree_Success();
    free_ptrLessPageSizeNoPrevNextAloc_Success();
    free_ptrLessPageSizeWithPrevAloc_Success();
    free_ptrLessPageSizeNoPrev_Success();

    printf("\n");
}
