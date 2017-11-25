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
    void  *data;
    size_t  size;
} CacheBlock;
typedef CacheBlock *CacheBlockPtr;

typedef struct CacheFlags{
    u_long referenced;
    unsigned char modified: 1;
} CacheFlags;

typedef struct CacheEntry{
    CacheBlock block;
    PA ram_pa;
    CacheFlags flags;
    struct CacheEntry *prev;
    struct CacheEntry *next;
} CacheEntry;
typedef CacheEntry *CacheNodePtr;

typedef struct Cache{
    CacheEntry *head;
    CacheEntry *tail;
    size_t capacity;
    size_t avail_size;
} Cache;
typedef Cache *CachePtr;


void init_cache(CachePtr cache, size_t size);
void destroy_cache(CachePtr cache);

void push_new_entry(CachePtr cache, CacheEntry *entry);
SearchRes search_in_cache(CachePtr table, PA addr, CacheNodePtr *node);
void read_from_cache(CacheNodePtr node, PA pa, void *pBuffer, size_t szBuffer);
void write_to_cache(CacheNodePtr node, PA pa, void *pBuffer, size_t szBuffer);

#endif //MM_CACHE_H
