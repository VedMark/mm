#ifndef MM_RAM_H
#define MM_RAM_H

#ifndef PHYS_ADDR
#define PHYS_ADDR
typedef char *PA;
typedef const PA const_PA;
#endif

#ifndef VIRT_ADDR
#define VIRT_ADDR
typedef char *VA;
typedef const VA const_VA;
#endif

typedef char *Data;

typedef struct MemPage{
    VA page_va;
    Data data;
} MemBlock;
typedef MemBlock *MemPagePtr;

typedef struct Memory {
    Data field;
    size_t size;
} Memory;
typedef Memory *MemoryPtr;

void init_ram(MemoryPtr memory, size_t szMemory);
void destroy_ram(MemoryPtr memory);
void *read_from_memory(PA pa, void *buff, size_t szBuffer);
void write_to_memory(PA pa, const void *buff, size_t szBuffer);

#endif //MM_RAM_H
