#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>

#include "include/tests.h"
#include "../include/err_codes.h"
#include "../include/mmemory.h"

void read_NullPtr_WrongParams(void) {
    int ret_code = 0;
    VA *ptr = malloc(sizeof(VA));
    init_(64, 64);
    malloc_(ptr, 32);
    char *pBuffer1 = "Stranger things";

    ret_code = read_(NULL, pBuffer1, strlen(pBuffer1));

    destroy_();
    free(ptr);
    assert(ret_code == EWRPAR);
    if(ret_code == EWRPAR)
        printf("%s: PASSED\n",__FUNCTION__);
}

void read_PtrIncorrectAddress_OutOfBorderErr(void) {
    int ret_code = 0;
    VA *ptr = malloc(sizeof(VA));
    init_(64, 64);
    malloc_(ptr, 32);
    char *pBuffer1 = "Stranger things";
    *ptr += 40;

    ret_code = read_(*ptr, pBuffer1, strlen(pBuffer1));

    destroy_();
    free(ptr);
    assert(ret_code == EOBORD);
    if(ret_code == EOBORD)
        printf("%s: PASSED\n",__FUNCTION__);
}

void read_20BytesTo10BytesBlock_OutOfBorderErr(void) {
    int ret_code = 0;
    VA *ptr = malloc(sizeof(VA));
    init_(64, 64);
    malloc_(ptr, 32);
    char *pBuffer1 = "Stranger things";

    ret_code = read_(*ptr, pBuffer1, 33);

    destroy_();
    free(ptr);
    assert(ret_code == EOBORD);
    if(ret_code == EOBORD)
        printf("%s: PASSED\n",__FUNCTION__);
}

void read_BlockIsNoInCache_Success(void) {
    int ret_code = 0;
    VA *ptr = malloc(sizeof(VA));
    init_(64, 64);
    malloc_(ptr, 32);
    char *pwBuffer = "Stranger things";
    char *prBuffer = malloc(strlen(pwBuffer) + 1);

    write_(*ptr, pwBuffer, strlen(pwBuffer) + 1);
    ret_code = read_(*ptr, prBuffer, strlen(pwBuffer) + 1);

    int cmp = strcmp(pwBuffer, prBuffer);
    destroy_();
    free(ptr);
    free(prBuffer);
    assert(ret_code == SUCCESS && !cmp);
    if(ret_code == SUCCESS && !cmp)
        printf("%s: PASSED\n",__FUNCTION__);
}

void read_BlockNoInCacheMoreThenCacheSize_Success(void) {
    int ret_code = 0;
    VA *ptr = malloc(sizeof(VA));
    init_(2048, 1024);
    malloc_(ptr, 513 * 1024);
    char *pwBuffer = malloc(513 * 1024);
    FILL_BUFFER(pwBuffer, 513 * 1024);
    char *prBuffer = malloc(513 * 1024);

    write_(*ptr, pwBuffer, strlen(pwBuffer) + 1);
    ret_code = read_(*ptr, prBuffer, strlen(pwBuffer) + 1);

    int cmp = strcmp(pwBuffer, prBuffer);
    destroy_();
    free(ptr);
    free(pwBuffer);
    free(prBuffer);
    assert(ret_code == SUCCESS && !cmp);
    if(ret_code == SUCCESS && !cmp)
        printf("%s: PASSED\n",__FUNCTION__);
}

void read_BlockNoInCacheCacheIsNotEmpty_Success(void) {
    int ret_code = 0;
    VA *ptr = malloc(sizeof(VA));
    char *pwBuffer = "Stranger things";
    init_(64, 64);
    malloc_(ptr, 32);
    write_(*ptr, pwBuffer, strlen(pwBuffer) + 1);
    pwBuffer = "Game of thrones";
    char *prBuffer = malloc(strlen(pwBuffer) + 1);

    write_(*ptr, pwBuffer, strlen(pwBuffer) + 1);
    ret_code = read_(*ptr, prBuffer, strlen(pwBuffer) + 1);

    int cmp = strcmp(pwBuffer, prBuffer);
    destroy_();
    free(ptr);
    free(prBuffer);
    assert(ret_code == SUCCESS && !cmp);
    if(ret_code == SUCCESS && !cmp)
        printf("%s: PASSED\n",__FUNCTION__);
}

void read_BlockIsInCache_Success(void) {
    int ret_code = 0;
    VA *ptr = malloc(sizeof(VA));
    init_(64, 64);
    malloc_(ptr, 32);
    char *pwBuffer = "Stranger things";
    write_(*ptr, pwBuffer, strlen(pwBuffer) + 1);
    char *prBuffer = malloc(strlen(pwBuffer) + 1);

    write_(*ptr, pwBuffer, strlen(pwBuffer) + 1);
    ret_code = read_(*ptr, prBuffer, strlen(pwBuffer) + 1);

    int cmp = strcmp(pwBuffer, prBuffer);
    destroy_();
    free(ptr);
    free(prBuffer);
    assert(ret_code == SUCCESS && !cmp);
    if(ret_code == SUCCESS && !cmp)
        printf("%s: PASSED\n",__FUNCTION__);
}

void read_ToBlockMiddle_Success(void) {
    int ret_code = 0;
    VA *ptr = malloc(sizeof(VA));
    int arr1[] = {1, 2, 3, 4, 5, 6, 7, 8, 9 , 10};
    int a = 13;
    int b = 0;
    init_(64, 64);
    malloc_(ptr, 10 * sizeof(int));
    write_(*ptr, arr1, 10 * sizeof(int));
    write_(*ptr + 2 * sizeof(int), &a, sizeof(int));

    ret_code = read_(*ptr + 2 * sizeof(int), &b, sizeof(int));

    destroy_();
    free(ptr);
    assert(ret_code == SUCCESS && a == b);
    if(ret_code == SUCCESS && a == b)
        printf("%s: PASSED\n",__FUNCTION__);
}

void read_FromLoadedPage_Success(void) {
    int ret_code = 0;
    VA *ptr1 = malloc(sizeof(VA));
    VA *ptr2 = malloc(sizeof(VA));
    char *pwBuffer = "Stranger things";
    char *prBuffer = malloc(strlen(pwBuffer) + 1);
    init_(4, 0x800000);
    malloc_(ptr1, 0x800000);
    malloc_(ptr2, strlen(pwBuffer) + 1);
    write_(*ptr2, pwBuffer, strlen(pwBuffer) + 1);
    write_(*ptr1, pwBuffer, strlen(pwBuffer) + 1);

    ret_code = read_(*ptr2, prBuffer, strlen(pwBuffer) + 1);

    int cmp = strcmp(pwBuffer, prBuffer);
    destroy_();
    free(ptr1);
    free(ptr2);
    free(prBuffer);
    assert(ret_code == SUCCESS && !cmp);
    if(ret_code == SUCCESS && !cmp)
        printf("%s: PASSED\n",__FUNCTION__);
}

void read_FromMultiPageSecondIsLoaded_Success(void) {
    int ret_code = 0;
    VA *ptr = malloc(sizeof(VA));
    char *pwBuffer = malloc(0x1000000);
    char *prBuffer = malloc(0x1000000);
    FILL_BUFFER(pwBuffer, 0x1000000);
    init_(4, 0x800000);
    malloc_(ptr, 0x800000);
    malloc_(ptr, strlen(pwBuffer) + 1);
    write_(*ptr, pwBuffer, strlen(pwBuffer) + 1);
    read_(*ptr, pwBuffer, 10);

    ret_code = read_(*ptr, prBuffer, strlen(pwBuffer) + 1);

    int cmp = strcmp(pwBuffer, prBuffer);
    destroy_();
    free(ptr);
    free(pwBuffer);
    free(prBuffer);
    assert(ret_code == SUCCESS && !cmp);
    if(ret_code == SUCCESS && !cmp)
        printf("%s: PASSED\n",__FUNCTION__);
}

void read_FullBlock_Success(void) {
    int ret_code = 0;
    VA *ptr = malloc(sizeof(VA));
    char *pwBuffer = "Stranger things";
    char *prBuffer = malloc(strlen(pwBuffer) + 1);
    init_(64, 64);
    malloc_(ptr, strlen(pwBuffer) + 1);
    write_(*ptr, pwBuffer, strlen(pwBuffer) + 1);

    ret_code = read_(*ptr, prBuffer, strlen(pwBuffer) + 1);

    int cmp = strcmp(pwBuffer, prBuffer);
    destroy_();
    free(ptr);
    free(prBuffer);
    assert(ret_code == SUCCESS && !cmp);
    if(ret_code == SUCCESS && !cmp)
        printf("%s: PASSED\n",__FUNCTION__);
}

void run_read_tests(void) {
    printf("UNIT TESTS: read_\n");

    read_NullPtr_WrongParams();
    read_PtrIncorrectAddress_OutOfBorderErr();
    read_20BytesTo10BytesBlock_OutOfBorderErr();
    read_BlockIsNoInCache_Success();
    read_BlockNoInCacheMoreThenCacheSize_Success();
    read_BlockNoInCacheCacheIsNotEmpty_Success();
    read_BlockIsInCache_Success();
    read_ToBlockMiddle_Success();
    read_FromLoadedPage_Success();
    read_FromMultiPageSecondIsLoaded_Success();
    read_FullBlock_Success();

    printf("\n");
}
