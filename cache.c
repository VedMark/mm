#include <malloc.h>
#include <memory.h>
#include <zconf.h>
#include "include/cache.h"

void remove_least_significant_block(CachePtr cache);
void remove_entry(CachePtr cache, CacheEntry *entry);

void init_cache(CachePtr cache, size_t size) {
    cache->head = NULL;
    cache->tail = NULL;
    cache->capacity = size;
    cache->avail_size = size;
}

void destroy_cache(CachePtr cache) {
    CacheEntry *entry = cache->head;
    while(NULL != entry) {
        cache->head = entry->next;
        free(entry->block.data);
        free(entry);
        entry = cache->head;
    }
}

void push_new_entry(CachePtr cache, CacheEntryPtr entry) {
    if(entry->block.size > cache->capacity) {
        return;
    }
    while(entry->block.size > cache->avail_size){
        remove_least_significant_block(cache);
    }
    entry->flags = (CacheFlags){0, 0};
    entry->prev = cache->tail;
    entry->next = NULL;
    if(cache->head == NULL) {
        cache->head = entry;
    } else {
        cache->tail->next = entry;
    }
    cache->tail = entry;
    cache->avail_size -= entry->block.size;
}

SearchRes search_in_cache(register CachePtr table, register VA addr, register CacheEntryPtr *entry) {
    register CacheEntryPtr ptr = table->head;
    while(NULL != ptr) {
        if(ptr->virt_addr <= addr && addr < ptr->virt_addr + ptr->block.size) {
            *entry = ptr;
            return CACHE_HIT;
        }
        ptr = ptr->next;
    }

    return CACHE_MISS;
}

void read_from_cache(CacheEntryPtr node,
                     VA va,
                     void *pBuffer,
                     size_t szBuffer) {

    memcpy(pBuffer, node->block.data + (va - node->virt_addr), szBuffer);
    node->flags.referenced += 1;
}

void write_to_cache(CacheEntryPtr node,
                    VA va,
                    void *pBuffer,
                    size_t szBuffer) {

    memcpy(node->block.data + (va - node->virt_addr), pBuffer, szBuffer);
    node->flags.referenced += 1;
    node->flags.modified = true;
}

void remove_loaded_blocks(register CachePtr cache, register VA page_va, register size_t szPage) {
    register CacheEntry *ptr = cache->head;

    while(NULL != ptr) {
        if(ptr->virt_addr <= page_va && page_va < ptr->virt_addr + ptr->block.size ||
           ptr->virt_addr <= page_va+szPage-1 && page_va+szPage-1 < ptr->virt_addr+ptr->block.size ||
           page_va <= ptr->virt_addr && ptr->virt_addr <= page_va + szPage ||
           page_va <= ptr->virt_addr+ptr->block.size-1 && ptr->virt_addr+ptr->block.size-1 <=page_va+szPage) {
            remove_entry(cache, ptr);
        }
        ptr = ptr->next;
    }
}

void remove_least_significant_block(register CachePtr cache) {
    register CacheEntry *blockPtr = cache->head;
    register CacheEntry *entry = cache->head;
    register u_long min = cache->head->flags.referenced;

    while(NULL != blockPtr) {
        if(min > blockPtr->flags.referenced) {
            min = blockPtr->flags.referenced;
            entry = blockPtr;
        }
        blockPtr = blockPtr->next;
    }
    remove_entry(cache, entry);
}

void remove_entry(register CachePtr cache, register CacheEntry *entry) {
    cache->avail_size += entry->block.size;
    if(entry == cache->head)
        cache->head = entry->next;
    if(entry == cache->tail)
        cache->tail = entry->prev;
    if(NULL != entry->next)
        entry->next->prev = entry->prev;
    if(NULL != entry->prev)
        entry->prev->next = entry->next;
    free(entry);
}
