#ifndef MM_CACHE_H
#define MM_CACHE_H

#include <stddef.h>
#include "page_segment.h"

#ifndef PHYS_ADDR
#define PHYS_ADDR
typedef char *PA;
#endif

typedef enum SearchRes{CACHE_HIT, CACHE_MISS} SearchRes;

typedef struct CacheBlock{
    char  *data;
    size_t  size;
} CacheBlock;
typedef CacheBlock *CacheBlockPtr;

typedef struct CacheFlags{
    u_long referenced;
    unsigned char modified: 1;
} CacheFlags;

typedef struct CacheEntry{
    CacheBlock block;
    VA virt_addr;
    CacheFlags flags;
    struct CacheEntry *prev;
    struct CacheEntry *next;
} CacheEntry;
typedef CacheEntry *CacheEntryPtr;

typedef struct Cache{
    CacheEntry *head;
    CacheEntry *tail;
    size_t capacity;
    size_t avail_size;
} Cache;
typedef Cache *CachePtr;


void init_cache(CachePtr cache, size_t size);
void destroy_cache(CachePtr cache);

void push_new_entry(CachePtr cache, CacheEntryPtr entry);
SearchRes search_in_cache(CachePtr table, VA addr, CacheEntryPtr *entry);
void read_from_cache(CacheEntryPtr node, VA va, void *pBuffer, size_t szBuffer);
void write_to_cache(CacheEntryPtr node, VA va, void *pBuffer, size_t szBuffer);
void remove_loaded_blocks(CachePtr cache, VA page_va, size_t szPage);

#endif //MM_CACHE_H
