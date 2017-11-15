#include <malloc.h>
#include <zconf.h>

#include "include/page_segment.h"


int init_tables(SegmentTablePtr table, u_int seg_count, u_int n, size_t szPage, PA base_addr) {
    table->table = (SegmentNodePtr) calloc(seg_count, sizeof(SegmentNode));
    if (NULL == table->table) return 1;
    table->szPage = szPage;

    for(u_int i = 0; i < seg_count; ++i) {
        table->table[i].descriptor.base_addr = i * n * szPage + base_addr;
        table->table[i].descriptor.size = i * n * szPage;
        table->table[i].descriptor.flags = (SegmentFlags){0, 0, 0};
        table->table[i].segment.page_table.table = (PageNodePtr) calloc((size_t) n, sizeof(PageNode));
        if(NULL == table->table[i].segment.page_table.table) return 1;
        for(u_int j = 0; j < n; j++) {
            PA pa = table->table[i].descriptor.base_addr;
            table->table[i].segment.page_table.table[j].descriptor.phys_addr = pa + j * szPage;
            table->table[i].segment.page_table.table[j].descriptor.flags = (PageFlags) {0, 0, 0};
        }
    }
}

void destroy_tables(SegmentTablePtr table) {
    for(u_int i = 0; i < table->count; ++i) {
        free(table->table[i].segment.page_table.table);
    }
    free(table->table);
}