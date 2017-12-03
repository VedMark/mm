#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "include/tests.h"
#include "../include/err_codes.h"
#include "../include/mmemory.h"

void malloc_0Bytes_WrongParams(void){
    int ret_code = 0;
    VA *ptr = malloc(sizeof(VA));
    init_(64, 64);

    ret_code = malloc_(ptr, 0);

    destroy_();
    free(ptr);
    assert(ret_code == EWRPAR);
    if(ret_code == EWRPAR)
        printf("%s: PASSED\n",__FUNCTION__);
}

void malloc_16384Bytes_MemoryLack(void){
    int ret_code = 0;
    VA *ptr = malloc(sizeof(VA));
    init_(64, 64);

    ret_code = malloc_(ptr, 128 * 128);

    destroy_();
    free(ptr);
    assert(ret_code == EMLACK);
    if(ret_code == EMLACK)
        printf("%s: PASSED\n",__FUNCTION__);
}

void malloc_FirstPtrMorePageSizeBlock_Success(void){
    int ret_code = 0;
    VA *ptr = malloc(sizeof(VA));
    init_(64, 64);

    ret_code = malloc_(ptr, 120);

    destroy_();
    free(ptr);
    assert(ret_code == SUCCESS);
    if(ret_code == SUCCESS)
        printf("%s: PASSED\n",__FUNCTION__);
}

void malloc_SecondPtrMorePageSizeBlock_Success(void){
    int ret_code = 0;
    VA *ptr = malloc(sizeof(VA));
    init_(64, 64);

    malloc_(ptr, 120);
    ret_code = malloc_(ptr, 100);

    destroy_();
    free(ptr);
    assert(ret_code == SUCCESS);
    if(ret_code == SUCCESS)
        printf("%s: PASSED\n",__FUNCTION__);
}

void malloc_PtrBetweenAllocatedMorePageSizeBlockBestFit_Success(void) {
    int ret_code = 0;
    VA *ptr1 = malloc(sizeof(VA));
    VA *ptr2 = malloc(sizeof(VA));
    init_(64, 64);

    malloc_(ptr1, 32);
    malloc_(ptr1, 64);
    malloc_(ptr2, 32);
    free_(*ptr1);
    malloc_(ptr1, 64);

    destroy_();
    free(ptr1);
    free(ptr2);
    assert(ret_code == SUCCESS);
    if(ret_code == SUCCESS)
        printf("%s: PASSED\n",__FUNCTION__);
}

void malloc_PtrBetweenAllocatedMorePageSizeBlockFit_Success(void) {
    int ret_code = 0;
    VA *ptr1 = malloc(sizeof(VA));
    VA *ptr2 = malloc(sizeof(VA));
    init_(64, 64);

    malloc_(ptr1, 32);
    malloc_(ptr1, 64);
    malloc_(ptr2, 32);
    free_(*ptr1);
    malloc_(ptr1, 60);

    destroy_();
    free(ptr1);
    free(ptr2);
    assert(ret_code == SUCCESS);
    if(ret_code == SUCCESS)
        printf("%s: PASSED\n",__FUNCTION__);
}

void malloc_PtrAfterAlocFreeAlocMorePageSizeBlock_Success(void) {
    int ret_code = 0;
    VA *ptr1 = malloc(sizeof(VA));
    VA *ptr2 = malloc(sizeof(VA));
    init_(64, 64);

    malloc_(ptr1, 32);
    malloc_(ptr1, 64);
    malloc_(ptr2, 32);
    free_(*ptr1);
    malloc_(ptr1, 65);

    destroy_();
    free(ptr1);
    free(ptr2);
    assert(ret_code == SUCCESS);
    if(ret_code == SUCCESS)
        printf("%s: PASSED\n",__FUNCTION__);
}

void malloc_FirstPtrEqualDoublePageSizeBlock_Success(void){
    int ret_code = 0;
    VA *ptr = malloc(sizeof(VA));
    init_(64, 64);

    ret_code = malloc_(ptr, 128);

    destroy_();
    free(ptr);
    assert(ret_code == SUCCESS);
    if(ret_code == SUCCESS)
        printf("%s: PASSED\n",__FUNCTION__);
}

void malloc_FirstPtrLessPageSizeBlock_Success(void){
    int ret_code = 0;
    VA *ptr = malloc(sizeof(VA));
    init_(64, 64);

    ret_code = malloc_(ptr, 32);

    destroy_();
    free(ptr);
    assert(ret_code == SUCCESS);
    if(ret_code == SUCCESS)
        printf("%s: PASSED\n",__FUNCTION__);
}

malloc_PtrBetweenAllocatedLessPageSizeBlockBestFit_Success() {
    int ret_code = 0;
    VA *ptr1 = malloc(sizeof(VA));
    VA *ptr2 = malloc(sizeof(VA));
    init_(64, 64);

    malloc_(ptr1, 20);
    malloc_(ptr1, 20);
    malloc_(ptr2, 20);
    free_(*ptr1);
    malloc_(ptr1, 20);

    destroy_();
    free(ptr1);
    free(ptr2);
    assert(ret_code == SUCCESS);
    if(ret_code == SUCCESS)
        printf("%s: PASSED\n",__FUNCTION__);
}

malloc_PtrBetweenAllocatedLessPageSizeBlockFit_Success() {
    int ret_code = 0;
    VA *ptr1 = malloc(sizeof(VA));
    VA *ptr2 = malloc(sizeof(VA));
    init_(64, 64);

    malloc_(ptr1, 20);
    malloc_(ptr1, 20);
    malloc_(ptr2, 20);
    free_(*ptr1);
    malloc_(ptr1, 10);

    destroy_();
    free(ptr1);
    free(ptr2);
    assert(ret_code == SUCCESS);
    if(ret_code == SUCCESS)
        printf("%s: PASSED\n",__FUNCTION__);
}

void malloc_SecondPtrLessPageSizeBlockNoSpaceAtEnd_Success(void){
    int ret_code = 0;
    VA *ptr = malloc(sizeof(VA));
    init_(64, 64);

    malloc_(ptr, 32);
    ret_code = malloc_(ptr, 32);

    destroy_();
    free(ptr);
    assert(ret_code == SUCCESS);
    if(ret_code == SUCCESS)
        printf("%s: PASSED\n",__FUNCTION__);
}

void malloc_SecondPtrLessPageSizeBlockWithSpaceAtEnd_Success(void){
    int ret_code = 0;
    VA *ptr = malloc(sizeof(VA));
    init_(64, 64);

    malloc_(ptr, 32);
    ret_code = malloc_(ptr, 30);

    destroy_();
    free(ptr);
    assert(ret_code == SUCCESS);
    if(ret_code == SUCCESS)
        printf("%s: PASSED\n",__FUNCTION__);
}

void malloc_TrirdPtrLessPageSizeBlockLeaveEmptySpace_Success(void){
    int ret_code = 0;
    VA *ptr = malloc(sizeof(VA));
    init_(64, 64);

    malloc_(ptr, 32);
    malloc_(ptr, 20);
    ret_code = malloc_(ptr, 20);

    destroy_();
    free(ptr);
    assert(ret_code == SUCCESS);
    if(ret_code == SUCCESS)
        printf("%s: PASSED\n",__FUNCTION__);
}

void malloc_PtrLessPageSizeBlockAfterAlocFreeAloc_Success(void){
    int ret_code = 0;
    VA *ptr1 = malloc(sizeof(VA));
    VA *ptr2 = malloc(sizeof(VA));
    init_(64, 64);

    malloc_(ptr1, 20);
    malloc_(ptr1, 20);
    malloc_(ptr2, 20);
    free_(*ptr1);
    ret_code = malloc_(ptr1, 30);

    destroy_();
    free(ptr1);
    free(ptr2);
    assert(ret_code == SUCCESS);
    if(ret_code == SUCCESS)
        printf("%s: PASSED\n",__FUNCTION__);
}

void run_malloc_tests(void) {
    printf("UNIT TESTS: malloc_\n");

    malloc_0Bytes_WrongParams();
    malloc_16384Bytes_MemoryLack();
    malloc_FirstPtrMorePageSizeBlock_Success();
    malloc_SecondPtrMorePageSizeBlock_Success();
    malloc_PtrBetweenAllocatedMorePageSizeBlockBestFit_Success();
    malloc_PtrBetweenAllocatedMorePageSizeBlockFit_Success();
    malloc_PtrAfterAlocFreeAlocMorePageSizeBlock_Success();
    malloc_FirstPtrEqualDoublePageSizeBlock_Success();
    malloc_FirstPtrLessPageSizeBlock_Success();
    malloc_PtrBetweenAllocatedLessPageSizeBlockBestFit_Success();
    malloc_PtrBetweenAllocatedLessPageSizeBlockFit_Success();
    malloc_SecondPtrLessPageSizeBlockNoSpaceAtEnd_Success();
    malloc_SecondPtrLessPageSizeBlockWithSpaceAtEnd_Success();
    malloc_TrirdPtrLessPageSizeBlockLeaveEmptySpace_Success();
    malloc_PtrLessPageSizeBlockAfterAlocFreeAloc_Success();

    printf("\n");
}
