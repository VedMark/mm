#include <stdio.h>
#include <malloc.h>

#include "include/mmemory.h"

int main() {
    init_(20, 4);
    VA ptr1 = NULL;
    VA ptr2 = NULL;
    VA ptr3 = NULL;
    VA ptr4 = NULL;
    VA ptr5 = NULL;
    VA ptr6 = NULL;
    malloc_(&ptr1, 3);
    printf("VA - %lu\n", (unsigned long)ptr1);
    malloc_(&ptr2, 6);
    printf("VA - %lu\n", (unsigned long)ptr2);
    malloc_(&ptr3, 4);
    printf("VA - %lu\n", (unsigned long)ptr3);
    malloc_(&ptr4, 2);
    printf("VA - %lu\n", (unsigned long)ptr4);
    malloc_(&ptr5, 2);
    printf("VA - %lu\n", (unsigned long)ptr5);
    malloc_(&ptr6, 10);

    malloc_(&ptr2, 6);
    printf("VA - %lu\n", (unsigned long)ptr2);
    malloc_(&ptr3, 4);
    printf("VA - %lu\n", (unsigned long)ptr3);
    malloc_(&ptr4, 2);
    printf("VA - %lu\n", (unsigned long)ptr4);
    malloc_(&ptr5, 2);
    printf("VA - %lu\n", (unsigned long)ptr5);
    malloc_(&ptr6, 10);

    malloc_(&ptr2, 6);
    printf("VA - %lu\n", (unsigned long)ptr2);
    malloc_(&ptr3, 4);
    printf("VA - %lu\n", (unsigned long)ptr3);
    malloc_(&ptr4, 2);
    printf("VA - %lu\n", (unsigned long)ptr4);
    malloc_(&ptr5, 2);
    printf("VA - %lu\n", (unsigned long)ptr5);
    malloc_(&ptr6, 10);
    printf("VA - %lu\n", (unsigned long)ptr6);
    malloc_(&ptr6, 10);
    printf("VA - %lu\n", (unsigned long)ptr6);
    free_(ptr1);
    free_(ptr2);
    free_(ptr5);
    free_(ptr6);
    free_(ptr3);
    free_(ptr4);
    printf("Hello, World!\n");
    return 0;
}
