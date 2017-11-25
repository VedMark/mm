#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include <zconf.h>

#include "include/mmemory.h"

int main() {
    init_(12, 10);

    VA ptr1 = NULL;
    VA ptr2 = NULL;
    malloc_(&ptr1, 3);
    printf("VA - %lu\n", (unsigned long)ptr1);


    char *string = "stranger things";
    malloc_(&ptr2, strlen(string));
    printf("VA - %lu\n", (unsigned long)ptr2);
    write_(ptr2, (void *) string, strlen(string));

    char *string1 = malloc(sizeof(char));
    read_(ptr2, string1, strlen(string));
    read_(ptr2, string1, strlen(string));

    free_(ptr1);
    free_(ptr2);

    free(string1);

    destroy_();
    return 0;
}
