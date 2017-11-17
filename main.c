#include <stdio.h>

#include "include/mmemory.h"

int main() {
    init_(4, 2);
    VA ptr1 = NULL;
    VA ptr2 = NULL;
    malloc_(&ptr1, 1);
    printf("VA - %lu\n", (unsigned long)ptr1);
    malloc_(&ptr2, 3);
    printf("VA - %lu\n", (unsigned long)ptr2);
    destroy_();
    printf("Hello, World!\n");
    return 0;
}
