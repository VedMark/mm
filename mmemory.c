#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zconf.h>

#include "include/assertion.h"
#include "include/cache.h"
#include "include/hard_drive.h"
#include "include/mmemory.h"
#include "include/page_segment.h"
#include "include/ram.h"


#define SUCCESS (0)
#define WRONG_PARAMETERS_ERROR (-1)
#define MEMORY_LACK_ERROR (-2)
#define OUTSIDE_BORDER_ERROR (-2)
#define UNKNOWN_ERROR (1)

#define RAM_MEM_SIZE ((16) *(USHRT_MAX + (1)))
#define HDD_MEM_SIZE ((16) * (RAM_MEM_SIZE))
#define HDD_LOCATION "/tmp/mm_hard_drive"
#define SEG_COUNT (2)


Memory g_RAM;
HardDrive g_HDD;
Cache g_cache;
SegmentTable g_segmentTable;

PA va_to_pa(VA ptr) {
    PA pa = 0;

}

int malloc_(VA *ptr, size_t szBlock) {
    // TODO: how can I choose an appropriate segment?
    ASSERT(0 < szBlock, "Invalid parameters", WRONG_PARAMETERS_ERROR);

    VA va = NULL;
    int ret_val = alloc_block(&g_segmentTable, szBlock, &va);
    ASSERT(-2 != ret_val, "Memory lack", MEMORY_LACK_ERROR);
    ASSERT(1 != ret_val, strerror(errno), UNKNOWN_ERROR);
    *ptr = va;
    print_all_what_I_need(&g_segmentTable);

    return SUCCESS;
}

int free_(VA ptr){
    return 0;
}

int read_(VA ptr, void *pBuffer, size_t szBuffer){
    // TODO: check if ptr is unavailable after deleting;
    return 0;
}

int write_(VA ptr, void *pBuffer, size_t szBuffer) {

    return 0;
}

int init_(int n, int szPage) {
    int ret_val = 0;

    ASSERT(0 < n && 0 < szPage, "Invalid parameters", WRONG_PARAMETERS_ERROR);

    init_ram(&g_RAM, RAM_MEM_SIZE, (size_t) szPage);
    ASSERT((g_RAM.field && g_RAM.pages), strerror(errno), UNKNOWN_ERROR);

    init_hard_drive(&g_HDD, HDD_LOCATION, HDD_MEM_SIZE);
    ASSERT_FILE(g_HDD.fd, -1 != g_HDD.fd, strerror(errno), UNKNOWN_ERROR);
    ASSERT((void *) -1 != g_HDD.data, strerror(errno), UNKNOWN_ERROR);

    ret_val = init_tables(&g_segmentTable, SEG_COUNT, (u_int) n, (size_t) szPage, g_RAM.field);
    ASSERT(1 != ret_val, strerror(errno), UNKNOWN_ERROR);
    g_segmentTable.seg_nodes[0].descriptor.rules = READ | EXEC;
    g_segmentTable.seg_nodes[1].descriptor.rules = READ | WRITE;
    print_all_what_I_need(&g_segmentTable);

    return SUCCESS;
}

void destroy_() {
    destroy_ram(&g_RAM);
    destroy_hard_drive(&g_HDD);
    destroy_tables(&g_segmentTable);
}