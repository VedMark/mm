#include "segment.h"
#include <stdlib.h>

int segment_isEmpty(SEGMENTNODEPTR headPtr) {
    return headPtr == NULL;
}

void segment_push(SEGMENTTABLEPTR table, SEGMENT value) {
    SEGMENTNODEPTR newPtr = malloc(sizeof(SEGMENTNODE));

    if(newPtr != NULL) {
        *newPtr = (SEGMENTNODE){
                .segment.base_addr = value.base_addr,
                .segment.segment_size = value.segment_size,
                .nextPtr = NULL
        };

        if(segment_isEmpty(table->head))
            table->head = newPtr;
        else
            table->tail->nextPtr = newPtr;

        table->tail = newPtr;
    }
    else {
        table->head = table->tail = newPtr;
    }
}

SEGMENT segment_pop(SEGMENTTABLEPTR table) {
    SEGMENT value;
    SEGMENTNODEPTR tempPtr;

    value = table->head->segment;
    tempPtr = table->head;
    table->head = table->head->nextPtr;

    if(table->head == NULL)
        table->tail = NULL;

    free(tempPtr);
    return value;
}
