#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/assertion.h"
#include "include/cache.h"
#include "include/hard_drive.h"
#include "include/mmemory.h"

#define RAM_MEM_SIZE (256 * (USHRT_MAX + (1)))
#define CACHE_MEM_SIZE (RAM_MEM_SIZE / (32))
#define HDD_MEM_SIZE (RAM_MEM_SIZE * 32)
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

    return SUCCESS;
}

int free_(VA ptr) {
    int ret_val = free_block(&g_segmentTable, ptr);
    ASSERT(EWRPAR != ret_val, "Invalid parameters", EWRPAR);
    ASSERT(EUNKNW != ret_val, "Unknown error", EUNKNW);
    return SUCCESS;
}

int read_(VA ptr, void *pBuffer, size_t szBuffer) {
    ASSERT(NULL != ptr && szBuffer > 0, "Invalid parameters", EWRPAR);

    u_int iSeg = 0;
    u_int iPage = 0;
    u_int offset = 0;
    int ret_code = 0;

    va_to_chunks(ptr, g_segmentTable.szPage, g_segmentTable.page_count,
                 &iSeg, &iPage, &offset);
    ASSERT((iSeg || iPage) &&
           iSeg < g_segmentTable.size &&
           iPage < g_segmentTable.seg_nodes[iSeg].segment->count, "Invalid parameters", EWRPAR);
    ASSERT(g_segmentTable.seg_nodes[iSeg].descriptor.rules & READ, "No rights to read", EUNKNW);

    CacheEntryPtr pCache_node = NULL;
    if(CACHE_HIT == search_in_cache(&g_cache, ptr, &pCache_node)) {
        ASSERT(ptr + szBuffer <= pCache_node->virt_addr + pCache_node->block.size,
               "Outside block access",
               EOBORD);
        read_from_cache(pCache_node, ptr, pBuffer, szBuffer);
    }
    else {
        ASSERT(validate_access(g_segmentTable.seg_nodes[iSeg].segment, iPage, offset, szBuffer),
               "Outside block access",
               EOBORD);

        if (g_segmentTable.seg_nodes[iSeg].segment->page_nodes[iPage].descriptor.flags.loaded) {
            remove_loaded_blocks(&g_cache,
                                 get_va(g_segmentTable.szPage, g_segmentTable.page_count, iSeg, iPage, 0),
                                 g_HDD.szPage);

            ret_code = unload_page(&g_segmentTable,
                                   iSeg,
                                   iPage,
                                   &g_HDD);
            ASSERT(EUNKNW == ret_code, "Unknown error", EUNKNW);
        }

        read_data(&g_segmentTable, iSeg, iPage, offset, pBuffer, szBuffer, &g_HDD);

        CacheEntryPtr cacheEntryPtr = read_block(&g_segmentTable, iSeg, iPage, offset, &g_HDD);
        if(!push_new_entry(&g_cache, cacheEntryPtr)) {
            free(cacheEntryPtr->block.data);
            free(cacheEntryPtr);
        };
    }

    return SUCCESS;
}

int write_(VA ptr, void *pBuffer, size_t szBuffer) {
    ASSERT(NULL != pBuffer && szBuffer > 0, "Invalid parameters", EWRPAR);

    u_int iSeg = 0;
    u_int iPage = 0;
    u_int offset = 0;
    int ret_code = 0;

    va_to_chunks(ptr, g_segmentTable.szPage, g_segmentTable.page_count,
                 &iSeg, &iPage, &offset);
    ASSERT((iSeg || iPage) &&
           iSeg < g_segmentTable.size &&
           iPage < g_segmentTable.seg_nodes[iSeg].segment->count, "Invalid parameters", EWRPAR);
    ASSERT(g_segmentTable.seg_nodes[iSeg].descriptor.rules & WRITE, "No rights to write", EUNKNW);
    ASSERT(validate_access(g_segmentTable.seg_nodes[iSeg].segment, iPage, offset, szBuffer),
           "Outside block access",
           EOBORD);

    if (g_segmentTable.seg_nodes[iSeg].segment->page_nodes[iPage].descriptor.flags.loaded) {
        remove_loaded_blocks(&g_cache,
                             get_va(g_segmentTable.szPage, g_segmentTable.page_count, iSeg, iPage, 0),
                             g_HDD.szPage);

        ret_code = unload_page(&g_segmentTable,
                               iSeg,
                               iPage,
                               &g_HDD);
        ASSERT(EUNKNW != ret_code, "Unknown error", EUNKNW);
    }

    write_data(&g_segmentTable, iSeg, iPage, offset, pBuffer, szBuffer, &g_HDD);

    CacheEntryPtr pCache_node = NULL;
    if(CACHE_HIT == search_in_cache(&g_cache, ptr, &pCache_node)) {
        ASSERT(ptr + szBuffer <= pCache_node->virt_addr + pCache_node->block.size,
               "Outside block access",
               EOBORD);
        write_to_cache(pCache_node, ptr, pBuffer, szBuffer);
    }
    else {
        CacheEntryPtr cacheEntryPtr = read_block(&g_segmentTable, iSeg, iPage, offset, &g_HDD);
        if(!push_new_entry(&g_cache, cacheEntryPtr)) {
            free(cacheEntryPtr->block.data);
            free(cacheEntryPtr);
        };
    }

    return SUCCESS;
}

int init_(int n, int szPage) {
    int ret_val = 0;

    ASSERT(0 < n && 0 < szPage && szPage <= RAM_MEM_SIZE, "Invalid parameters", EWRPAR);

    init_ram(&g_RAM, RAM_MEM_SIZE);
    ASSERT(g_RAM.field, strerror(errno), EUNKNW);

    init_hard_drive(&g_HDD, HDD_LOCATION, (size_t) szPage, HDD_MEM_SIZE);
    ASSERT_FILE(g_HDD.fd, -1 != g_HDD.fd, strerror(errno), EUNKNW);
    ASSERT((void *) -1 != g_HDD.data, strerror(errno), EUNKNW);

    init_cache(&g_cache, CACHE_MEM_SIZE);

    ret_val = init_tables(&g_segmentTable, SEG_COUNT, (u_int) n, (size_t) szPage, &g_HDD, &g_RAM);
    ASSERT(EUNKNW != ret_val, strerror(errno), EUNKNW);
    g_segmentTable.seg_nodes[0].descriptor.rules = READ | WRITE;
    g_segmentTable.seg_nodes[1].descriptor.rules = READ | WRITE;

    return SUCCESS;
}

void destroy_() {
    destroy_ram(&g_RAM);
    destroy_hard_drive(&g_HDD);
    destroy_cache(&g_cache);
    destroy_tables(&g_segmentTable);
}
