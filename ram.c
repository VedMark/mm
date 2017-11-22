#include <memory.h>
#include <stdlib.h>

#include "include/ram.h"


void init_ram(MemoryPtr memory, size_t szMemory) {
    *memory = (Memory){
            .field = (Data) malloc(szMemory * sizeof(char)),
            .size = szMemory
    };
}

void destroy_ram(MemoryPtr memory) {
    free(memory->field);
}

void *read_from_memory(PA pa, void *buff, const size_t szBuffer) {
    memcpy(buff, pa, szBuffer);
}

void write_to_memory(PA pa, const void *const buff, const size_t szBuffer) {
    memcpy(pa, buff, szBuffer);
}
