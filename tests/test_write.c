#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>

#include "include/tests.h"
#include "../include/err_codes.h"
#include "../include/mmemory.h"

void write_NullPtr_WrongParams(void) {
    int ret_code = 0;
    VA *ptr = malloc(sizeof(VA));
    init_(64, 64);
    malloc_(ptr, 32);
    char *pBuffer1 = "Stranger things";

    ret_code = write_(NULL, pBuffer1, strlen(pBuffer1));

    destroy_();
    free(ptr);
    assert(ret_code == EWRPAR);
    if(ret_code == EWRPAR)
        printf("%s: PASSED\n",__FUNCTION__);
}

void write_PtrIncorrectAddress_OutOfBorderErr(void) {
    int ret_code = 0;
    VA *ptr = malloc(sizeof(VA));
    init_(64, 64);
    malloc_(ptr, 32);
    char *pBuffer1 = "Stranger things";
    *ptr += 40;

    ret_code = write_(*ptr, pBuffer1, strlen(pBuffer1));

    destroy_();
    free(ptr);
    assert(ret_code == EOBORD);
    if(ret_code == EOBORD)
        printf("%s: PASSED\n",__FUNCTION__);
}

void write_20BytesTo10BytesBlock_OutOfBorderErr(void) {
    int ret_code = 0;
    VA *ptr = malloc(sizeof(VA));
    init_(64, 64);
    malloc_(ptr, 32);
    char *pBuffer1 = "Stranger things";

    ret_code = write_(*ptr, pBuffer1, 33);

    destroy_();
    free(ptr);
    assert(ret_code == EOBORD);
    if(ret_code == EOBORD)
        printf("%s: PASSED\n",__FUNCTION__);
}

void write_BlockIsNoInCache_Success(void) {
    int ret_code = 0;
    VA *ptr = malloc(sizeof(VA));
    init_(64, 64);
    malloc_(ptr, 32);
    char *pBuffer = "Stranger things";

    ret_code = write_(*ptr, pBuffer, strlen(pBuffer) + 1);

    destroy_();
    free(ptr);
    assert(ret_code == SUCCESS);
    if(ret_code == SUCCESS)
        printf("%s: PASSED\n",__FUNCTION__);
}

void write_BlockNoInCacheMoreThenCacheSize_Success(void) {
    int ret_code = 0;
    VA *ptr = malloc(sizeof(VA));
    init_(2048, 1024);
    malloc_(ptr, 513 * 1024);
    char *pBuffer = malloc(513 * 1024);
    FILL_BUFFER(pBuffer, 513 * 1024);

    ret_code = write_(*ptr, pBuffer, 513 * 1024);

    destroy_();
    free(ptr);
    free(pBuffer);
    assert(ret_code == SUCCESS);
    if(ret_code == SUCCESS)
        printf("%s: PASSED\n",__FUNCTION__);
}

void write_BlockNoInCacheCacheIsNotEmpty_Success(void) {
    int ret_code = 0;
    VA *ptr = malloc(sizeof(VA));
    char *pBuffer = "Stranger things";
    init_(64, 64);
    malloc_(ptr, 32);
    write_(*ptr, pBuffer, strlen(pBuffer) + 1);
    pBuffer = "Game of thrones";

    ret_code = write_(*ptr, pBuffer, strlen(pBuffer) + 1);

    destroy_();
    free(ptr);
    assert(ret_code == SUCCESS);
    if(ret_code == SUCCESS)
        printf("%s: PASSED\n",__FUNCTION__);
}

void write_BlockIsInCache_Success(void) {
    int ret_code = 0;
    VA *ptr = malloc(sizeof(VA));
    init_(64, 64);
    malloc_(ptr, 32);
    char *pBuffer = "Stranger things";
    write_(*ptr, pBuffer, strlen(pBuffer) + 1);

    ret_code = write_(*ptr, pBuffer, strlen(pBuffer) + 1);

    destroy_();
    free(ptr);
    assert(ret_code == SUCCESS);
    if(ret_code == SUCCESS)
        printf("%s: PASSED\n",__FUNCTION__);
}

void write_ToBlockMiddle_Success(void) {
    int ret_code = 0;
    VA *ptr = malloc(sizeof(VA));
    int arr1[] = {1, 2, 3, 4, 5, 6, 7, 8, 9 , 10};
    int a = 13;
    init_(64, 64);
    malloc_(ptr, 10 * sizeof(int));
    write_(*ptr, arr1, 10 * sizeof(int));

    ret_code = write_(*ptr + 2 * sizeof(int), &a, sizeof(int));

    destroy_();
    free(ptr);
    assert(ret_code == SUCCESS);
    if(ret_code == SUCCESS)
        printf("%s: PASSED\n",__FUNCTION__);
}

void write_ToLoadedPage_Success(void) {
    int ret_code = 0;
    VA *ptr = malloc(sizeof(VA));
    char *pBuffer = "Stranger things";
    init_(4, 0x800000);
    malloc_(ptr, 0x800000);
    malloc_(ptr, strlen(pBuffer) + 1);

    ret_code = write_(*ptr, pBuffer, strlen(pBuffer) + 1);

    destroy_();
    free(ptr);
    assert(ret_code == SUCCESS);
    if(ret_code == SUCCESS)
        printf("%s: PASSED\n",__FUNCTION__);
}

void write_ToMultiPageSecondIsLoaded_Success(void) {
    int ret_code = 0;
    VA *ptr = malloc(sizeof(VA));
    char *pBuffer = malloc(0x1000000);
    FILL_BUFFER(pBuffer, 0x1000000);
    init_(4, 0x800000);
    malloc_(ptr, 0x800000);
    malloc_(ptr, 0x1000000);

    ret_code = write_(*ptr, pBuffer, 0x1000000);

    destroy_();
    free(ptr);
    free(pBuffer);
    assert(ret_code == SUCCESS);
    if(ret_code == SUCCESS)
        printf("%s: PASSED\n",__FUNCTION__);
}

void write_FullBlock_Success(void) {
    int ret_code = 0;
    VA *ptr = malloc(sizeof(VA));
    char *pBuffer = "Stranger things";
    init_(64, 64);
    malloc_(ptr, strlen(pBuffer) + 1);

    ret_code = write_(*ptr, pBuffer, strlen(pBuffer) + 1);

    destroy_();
    free(ptr);
    assert(ret_code == SUCCESS);
    if(ret_code == SUCCESS)
        printf("%s: PASSED\n",__FUNCTION__);
}

void run_write_tests(void) {
    printf("UNIT TESTS: write_\n");

    write_NullPtr_WrongParams();
    write_PtrIncorrectAddress_OutOfBorderErr();
    write_20BytesTo10BytesBlock_OutOfBorderErr();
    write_BlockIsNoInCache_Success();
    write_BlockNoInCacheMoreThenCacheSize_Success();
    write_BlockNoInCacheCacheIsNotEmpty_Success();
    write_BlockIsInCache_Success();
    write_ToBlockMiddle_Success();
    write_ToLoadedPage_Success();
    write_ToMultiPageSecondIsLoaded_Success();
    write_FullBlock_Success();

    printf("\n");
}
