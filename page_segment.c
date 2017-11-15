#include <malloc.h>
#include <zconf.h>

#include "include/page_segment.h"


int init_tables(SegmentTablePtr table, u_int seg_count, u_int n, size_t szPage, PA base_addr) {
    table->nodes = (SegmentNodePtr) calloc(seg_count, sizeof(SegmentNode));
    if (NULL == table->nodes) return 1;
    table->szPage = szPage;
    table->count = seg_count;

    for(u_int i = 0; i < seg_count; ++i) {
        table->nodes[i].descriptor.base_addr = i * n * szPage + base_addr;
        table->nodes[i].descriptor.size = i * n * szPage;
        table->nodes[i].descriptor.flags = (SegmentFlags){0, 0, 0};
        table->nodes[i].segment = (SegmentPtr) malloc(sizeof(Segment));
        table->nodes[i].segment->nodes = (PageNodePtr) calloc(n, sizeof(PageNode));
        table->nodes[i].segment->count = n;
        if(NULL == table->nodes[i].segment->nodes) return 1;
        for(u_int j = 0; j < n; j++) {
            PA pa = table->nodes[i].descriptor.base_addr;
            PageNodePtr nodes = table->nodes[i].segment->nodes;
            nodes[j].page.first_used = (Block *) malloc(sizeof(Block));
            nodes[j].page.first_free = (Block *) malloc(sizeof(Block));
            nodes[j].page.max_free_size = szPage;
            nodes[j].descriptor.phys_addr = pa + j * szPage;
            nodes[j].descriptor.flags = (PageFlags) {0, 0, 0};
        }
    }
}

void destroy_tables(SegmentTablePtr table) {
    Block *ptr = NULL;
    for(u_int i = 0; i < table->count; ++i) {
        for(u_int j = 0; j < table->nodes[i].segment->count; j++) {
            ptr = table->nodes[i].segment->nodes[j].page.first_used;
            while(NULL != ptr) {
                table->nodes[i].segment->nodes[j].page.first_used = ptr->next;
                free(ptr);
                ptr = table->nodes[i].segment->nodes[j].page.first_used;
            }
            ptr = table->nodes[i].segment->nodes[j].page.first_free;
            while(NULL != ptr) {
                table->nodes[i].segment->nodes[j].page.first_free = ptr->next;
                free(ptr);
                ptr = table->nodes[i].segment->nodes[j].page.first_free;
            }
        }
        free(table->nodes[i].segment->nodes);
        free(table->nodes[i].segment);
    }
    free(table->nodes);
}