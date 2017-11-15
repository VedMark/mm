#include <stdlib.h>

#include "include/ram.h"

size_t PAGE_SIZE;

void init_ram(MemoryPtr memory, size_t szMemory, size_t szPage) {
    PAGE_SIZE = szPage;
    *memory = (Memory){
            .field = (PA) calloc(szMemory, sizeof(char)),
            .pages = (PagePtr) calloc(szMemory / (size_t) szPage, sizeof(Page)),
            .page_count = szMemory / (size_t) szPage
    };
}

void destroy_ram(MemoryPtr memory) {
    free(memory->field);
    free(memory->pages);
}