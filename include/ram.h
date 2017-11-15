#ifndef MEMOMY_MANAGER_RAM_H
#define MEMOMY_MANAGER_RAM_H

#include "page_segment.h"

typedef struct Memory{
    PA field;
    PagePtr pages;
    size_t page_count;
} Memory;
typedef Memory *MemoryPtr;

void init_ram(MemoryPtr, size_t szMemory, size_t szPage);
void destroy_ram(MemoryPtr);

#endif //MEMOMY_MANAGER_RAM_H
