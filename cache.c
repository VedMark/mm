#include <malloc.h>
#include <memory.h>
#include <zconf.h>
#include "include/cache.h"

void init_cache(CachePtr cache, size_t size) {
    cache->head = NULL;
    cache->tail = NULL;
    cache->capacity = size;
    cache->avail_size = size;
}

void destroy_cache(CachePtr cache) {
    CacheEntry *entry = cache->head;
    while(entry != cache->tail) {
        free(entry->block.data);
        free(entry);
    }
    free(cache->tail);
}

void remove_least_significant_block(CachePtr cache) {
    CacheEntry *blockPtr = cache->head;
    CacheEntry *remove_entry = cache->head;
    u_long min = cache->head->flags.referenced;

    while(NULL != blockPtr) {
        if(min > blockPtr->flags.referenced) {
            min = blockPtr->flags.referenced;
            remove_entry = blockPtr;
        }
        blockPtr = blockPtr->next;
    }
    cache->avail_size += remove_entry->block.size;
    if(blockPtr == cache->head)
        cache->head = remove_entry->next;
    if(blockPtr == cache->tail)
        cache->tail = remove_entry->prev;
    if(NULL != remove_entry->next)
        remove_entry->next->prev = remove_entry->prev;
    if(NULL != remove_entry->prev)
        remove_entry->prev->next = remove_entry->next;
    free(remove_entry);
}

void push_new_entry(CachePtr cache, CacheEntry *entry) {
    if(entry->block.size > cache->capacity) {
        return;
    }
    while(entry->block.size > cache->avail_size){
        remove_least_significant_block(cache);
    }
    entry->flags = (CacheFlags){0, 0};
    entry->prev = cache->tail;
    if(cache->head == NULL) {
        cache->head = entry;
    }
    cache->tail = entry;
    cache->avail_size -= entry->block.size;
}

SearchRes search_in_cache(register CachePtr table, register PA addr, register CacheNodePtr *node) {
    register CacheNodePtr ptr = table->head;
    while(NULL != ptr) {
        if(ptr->ram_pa <= addr && addr < ptr->ram_pa + ptr->block.size) {
            *node = ptr;
            return CACHE_HIT;
        }
        ptr = ptr->next;
    }

    return CACHE_MISS;
}

void read_from_cache(register CacheNodePtr node,
                     register PA pa,
                     register void *pBuffer,
                     register size_t szBuffer) {

    memcpy(pBuffer, node->block.data + (node->ram_pa - pa), szBuffer);
    node->flags.referenced += 1;
}

void write_to_cache(register CacheNodePtr node,
                    PA register pa,
                    register void *pBuffer,
                    register size_t szBuffer) {

    memcpy(node->block.data + (node->ram_pa - pa), pBuffer, szBuffer);
    node->flags.referenced += 1;
    node->flags.modified = true;
}
