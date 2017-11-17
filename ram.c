#include <stdlib.h>

#include "include/ram.h"

size_t PAGE_SIZE;

void init_ram(MemoryPtr memory, size_t szMemory, size_t szPage) {
    PAGE_SIZE = szPage;
    *memory = (Memory){
            .field = (data) malloc(szMemory * sizeof(char)),
            .size = szMemory,
            .pages = (MemPagePtr) malloc(szMemory / (size_t) szPage * sizeof(MemPage)),
            .szPage = szPage,
            .cPage = szMemory / (size_t) szPage
    };
}

void destroy_ram(MemoryPtr memory) {
    free(memory->field);
    free(memory->pages);
}