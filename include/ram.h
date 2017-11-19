#ifndef MM_RAM_H
#define MM_RAM_H

#ifndef PHYS_ADDR
#define PHYS_ADDR
typedef char *PA;
#endif

typedef void *data;

typedef PA MemPage;
typedef MemPage *MemPagePtr;

typedef struct Memory{
    data field;
    size_t size;
    MemPagePtr pages;
    size_t szPage;
    size_t cPage;
} Memory;
typedef Memory *MemoryPtr;

void init_ram(MemoryPtr, size_t szMemory, size_t szPage);
void destroy_ram(MemoryPtr);

#endif //MM_RAM_H
