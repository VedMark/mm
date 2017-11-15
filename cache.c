#include <malloc.h>
#include "include/cache.h"


int isEmpty(CachePtr cache) {
    return NULL == cache->head;
}

void push(CachePtr table, CacheBlock value, PA addr) {
    CacheNodePtr newPtr = malloc(sizeof(CacheEntry));

    if(NULL != newPtr) {
        *newPtr = (CacheEntry){
                .address = addr,
                .flags = {0},
                .next = NULL,
                .prev = table->tail
        };
        *newPtr->block = value;

        if(isEmpty(table))
            table->head = newPtr;
        else
            table->tail->next = newPtr;

        table->tail = newPtr;
    }
    else {
        table->head = table->tail = newPtr;
    }
}

CacheBlockPtr search(CachePtr table, PA addr) {
    CacheNodePtr ptr = table->head;
    while(NULL != ptr) {
        if(ptr->address == addr) {
            return ptr->block;
        }
        ++ptr;
    }
    return NULL;
}
