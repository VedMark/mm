#ifndef MEMOMY_MANAGER_CACHE_H
#define MEMOMY_MANAGER_CACHE_H

#include <stddef.h>

#ifndef PHYS_ADDR
typedef char *PA;
#endif


typedef struct CacheBlock{
    void  *data;
    size_t  size;
} CacheBlock;
typedef CacheBlock *CacheBlockPtr;

typedef struct CacheFlags{
    unsigned char modified: 1;
} CacheFlags;

typedef struct CacheEntry{
    PA address;
    CacheBlockPtr block;
    CacheFlags flags;
    struct CacheEntry *prev;
    struct CacheEntry *next;
} CacheEntry;
typedef CacheEntry *CacheNodePtr;

typedef struct Cache{
    CacheEntry *head;
    CacheEntry *tail;
} Cache;
typedef Cache *CachePtr;

int isEmpty(CachePtr);
void push(CachePtr, CacheBlock, PA);
CacheBlockPtr search(CachePtr table, PA addr);

#endif //MEMOMY_MANAGER_CACHE_H
