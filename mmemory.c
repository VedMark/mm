#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/assertion.h"
#include "include/cache.h"
#include "include/hard_drive.h"
#include "include/mmemory.h"
#include "include/page_segment.h"

#define RAM_MEM_SIZE ((USHRT_MAX + (1)))
#define HDD_MEM_SIZE (RAM_MEM_SIZE / (2))
#define HDD_LOCATION "/tmp/mm_hard_drive"
#define SEG_COUNT (2)


Memory g_RAM;
HardDrive g_HDD;
Cache g_cache;
SegmentTable g_segmentTable;


int malloc_(VA *ptr, size_t szBlock) {
    ASSERT(0 < szBlock, "Invalid parameters", EWRPAR);

    VA va = NULL;
    int ret_val = alloc_block(&g_segmentTable, szBlock, &va);
    ASSERT(EMLACK != ret_val, "Memory lack", EMLACK);
    ASSERT(EUNKNW != ret_val, strerror(errno), EUNKNW);
    *ptr = va;

    print_all_what_I_need(&g_segmentTable);

    return SUCCESS;
}

int free_(VA ptr){
    int ret_val = free_block(&g_segmentTable, ptr);
    ASSERT(EWRPAR != ret_val, "Invalid parameters", EWRPAR);
    ASSERT(EUNKNW != ret_val, "Unknown error", EUNKNW);
    return SUCCESS;
}

int read_(VA ptr, void *pBuffer, size_t szBuffer) {
    ASSERT(NULL != ptr && szBuffer > 0, "Invalid parameters", EWRPAR);

    u_int numSeg = 0;
    u_int numPage = 0;
    u_int offset = 0;

    va_to_chunks(ptr, g_segmentTable.szPage, g_segmentTable.szAddr,
                 &numSeg, &numPage, &offset);
    ASSERT(numSeg || numPage, "Invalid parameters", EWRPAR);
    ASSERT(validate_access(g_segmentTable.seg_nodes[numSeg].segment, numPage, offset, szBuffer),
           "Outside block reading",
           EOBORD);

    if (g_segmentTable.seg_nodes[numSeg].segment->page_nodes[numPage].descriptor.flags.loaded) {
        unload_page(&g_segmentTable,
                    numSeg,
                    numPage,
                    &g_HDD);
    }

    read_data(&g_segmentTable, numSeg, numPage, offset, pBuffer, szBuffer);

    return SUCCESS;
}

int write_(VA ptr, void *pBuffer, size_t szBuffer) {
    ASSERT(NULL != ptr && szBuffer > 0, "Invalid parameters", EWRPAR);

    u_int numSeg = 0;
    u_int numPage = 0;
    u_int offset = 0;

    va_to_chunks(ptr, g_segmentTable.szPage, g_segmentTable.szAddr,
                 &numSeg, &numPage, &offset);
    ASSERT(numSeg || numPage, "Invalid parameters", EWRPAR);
    ASSERT(validate_access(g_segmentTable.seg_nodes[numSeg].segment, numPage, offset, szBuffer),
           "Outside block reading",
           EOBORD);

    if(g_segmentTable.seg_nodes[numSeg].segment->page_nodes[numPage].descriptor.flags.loaded) {
        unload_page(&g_segmentTable,
                    numSeg,
                    numPage,
                    &g_HDD);
    }

    write_data(&g_segmentTable, numSeg, numPage, offset, pBuffer, szBuffer);

    return SUCCESS;
}

int init_(int n, int szPage) {
    int ret_val = 0;

    ASSERT(0 < n && 0 < szPage, "Invalid parameters", EWRPAR);

    init_ram(&g_RAM, RAM_MEM_SIZE);
    ASSERT(g_RAM.field, strerror(errno), EUNKNW);

    init_hard_drive(&g_HDD, HDD_LOCATION, (size_t) szPage, HDD_MEM_SIZE);
    ASSERT_FILE(g_HDD.fd, -1 != g_HDD.fd, strerror(errno), EUNKNW);
    ASSERT((void *) -1 != g_HDD.data, strerror(errno), EUNKNW);

    ret_val = init_tables(&g_segmentTable, SEG_COUNT, (u_int) n, (size_t) szPage, &g_HDD, &g_RAM);
    ASSERT(EUNKNW != ret_val, strerror(errno), EUNKNW);
    g_segmentTable.seg_nodes[0].descriptor.rules = READ | EXEC;
    g_segmentTable.seg_nodes[1].descriptor.rules = READ | WRITE;

    return SUCCESS;
}

void destroy_() {
    destroy_ram(&g_RAM);
    destroy_hard_drive(&g_HDD);
    destroy_tables(&g_segmentTable);
}
