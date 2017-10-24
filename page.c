#include "page.h"
#include <stdlib.h>

int page_isEmpty(PAGENODEPTR headPtr) {
    return headPtr == NULL;
}

void page_push(PAGETABLEPTR table, PAGE value) {
    PAGENODEPTR newPtr = malloc(sizeof(PAGENODE));

    if(newPtr != NULL) {
        *newPtr = (PAGENODE){
                .page.physical_page = value.physical_page,
                .page.flags = value.flags,
                .nextPtr = NULL
        };

        if(page_isEmpty(table->head))
            table->head = newPtr;
        else
            table->tail->nextPtr = newPtr;

        table->tail = newPtr;
    }
    else {
        table->head = table->tail = newPtr;
    }
}

PAGE page_pop(PAGETABLEPTR table) {
    PAGE value;
    PAGENODEPTR tempPtr;

    value = table->head->page;
    tempPtr = table->head;
    table->head = table->head->nextPtr;

    if(table->head == NULL)
        table->tail = NULL;

    free(tempPtr);
    return value;
}
