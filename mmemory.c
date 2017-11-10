#include <stdio.h>
#include <stdlib.h>

#include "mmemory.h"
#include "page.h"
#include "segment.h"

typedef char *PA;
typedef unsigned long long MemorySize;

typedef struct Memory{
    PA *baseAddr;
    MemorySize size;
} Memory;


Memory g_RAM;
SegmentTablePtr g_segmentTable;
PageTablePtr g_pageTable;


int _malloc (VA* ptr, size_t szBlock) {
    return 0;
}

int _free (VA ptr){
    return 0;
}

int _read (VA ptr, void* pBuffer, size_t szBuffer){
    return 0;
}

int _write(VA ptr, void *pBuffer, size_t szBuffer) {

    PageNodePtr page_node = (void*)malloc(sizeof(PageNode));

    return 0;
}

int init(int n, int szPage) {
    return 0;
}
