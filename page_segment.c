#include <malloc.h>
#include <stdbool.h>
#include <zconf.h>

#include "include/page_segment.h"

typedef enum Block_type{USED, FREE, ANY} Block_type;

int init_tables(SegmentTablePtr restrict table, u_int seg_count, u_int n, size_t szPage, PA base_addr) {
    table->seg_nodes = (SegmentNodePtr) calloc(seg_count, sizeof(SegmentNode));
    if (NULL == table->seg_nodes) return 1;
    table->szPage = szPage;
    table->count = seg_count;
    table->szAddr = (u_int) (n * szPage);
    n = n / seg_count;

    for(u_int i = 0; i < seg_count; ++i) {
        table->seg_nodes[i].descriptor.base_virt_addr = (VA) (i * n * szPage);
        table->seg_nodes[i].descriptor.base_phys_addr = i * n * szPage + base_addr;
        table->seg_nodes[i].descriptor.size = i * n * szPage;
        table->seg_nodes[i].descriptor.flags = (SegmentFlags){0, 0, 0};
        table->seg_nodes[i].segment = (SegmentPtr) malloc(sizeof(Segment));
        table->seg_nodes[i].segment->page_nodes = (PageNodePtr) calloc(n, sizeof(PageNode));
        table->seg_nodes[i].segment->count = n;
        if(NULL == table->seg_nodes[i].segment->page_nodes) return 1;
        for(u_int j = 0; j < n; j++) {
            PA pa = table->seg_nodes[i].descriptor.base_phys_addr;
            PageNodePtr nodes = table->seg_nodes[i].segment->page_nodes;
            nodes[j].page.first_block = (Block *) malloc(sizeof(Block));
            nodes[j].page.first_block->size = szPage;
            nodes[j].page.first_block->free = true;
            nodes[j].page.max_free_size = szPage;
            nodes[j].descriptor.phys_addr = pa + j * szPage;
            nodes[j].descriptor.flags = (PageFlags) {0, 0, 0};
        }
    }
}

void destroy_tables(SegmentTablePtr restrict table) {
    Block *ptr = NULL;
    for(u_int i = 0; i < table->count; ++i) {
        for(u_int j = 0; j < table->seg_nodes[i].segment->count; j++) {
            ptr = table->seg_nodes[i].segment->page_nodes[j].page.first_block;
            while(NULL != ptr) {
                table->seg_nodes[i].segment->page_nodes[j].page.first_block = ptr->next;
                free(ptr);
                ptr = table->seg_nodes[i].segment->page_nodes[j].page.first_block;
            }
        }
        free(table->seg_nodes[i].segment->page_nodes);
        free(table->seg_nodes[i].segment);
    }
    free(table->seg_nodes);
}


int found_multipage_space(const_PageTablePtr restrict page_table,
                          u_int index, size_t szBlock, int bFirst);
VA get_virtual_address(VA seg_addr, u_int numPage, size_t szPage, u_int szAddr, u_int offset);
u_int log_2(u_int num);
u_int get_best_fit_offset(const_PagePtr restrict ptrPage, size_t szBlock, Block **best_fit);
u_int get_offset(const_PagePtr ptrPage, const Block const *ptrBlock);
const Block *find_last(const_PagePtr restrict ptrPage, Block_type type);
int insert_block(PageTablePtr restrict table,
                  u_int j,
                  Block* restrict f_block,
                  size_t szBlock,
                  size_t szPage,
                  int to_many);
void set_max_free_size(PagePtr page);
Block *find_prev(const_PagePtr ptrPage, const Block const *f_block);

int alloc_block(SegmentTablePtr restrict table, size_t szBlock, VA *va) {
    u_int offset = 0;
    for(u_int i = 0; i < table->count; ++i) {
        SegmentPtr ptrSegment = table->seg_nodes[i].segment;
        for (u_int j = 0; j < ptrSegment->count; ++j) {
            if (szBlock > table->szPage) {
                if (found_multipage_space(ptrSegment, 0, szBlock, true)) {
                    offset = get_offset(&ptrSegment->page_nodes[j].page,
                                        find_last(&ptrSegment->page_nodes[j].page, ANY));
                    *va = get_virtual_address(table->seg_nodes[i].descriptor.base_virt_addr,
                                              j, table->szPage, table->szAddr, offset);
                    if(insert_block(table->seg_nodes[i].segment, j,
                                 (Block *) find_last(&ptrSegment->page_nodes[i].page, FREE),
                                 szBlock, table->szPage, true)) return 0;
                    else return 1;
                }
            }
            else {
                PagePtr ptrPage = &table->seg_nodes[i].segment->page_nodes[j].page;
                if (ptrPage->max_free_size >= szBlock) {
                    Block *best_block = NULL;
                    offset = get_best_fit_offset(ptrPage, szBlock, &best_block);
                    *va = get_virtual_address(table->seg_nodes[i].descriptor.base_virt_addr,
                                              j, table->szPage, table->szAddr, offset);
                    if(insert_block(table->seg_nodes[i].segment,
                                 j, best_block, szBlock, table->szPage, false)) return 0;
                    else return 1;
                }
            }
        }
    }
    return -2;
}

int found_multipage_space(const_PageTablePtr restrict page_table, u_int index, size_t szBlock, int bFirst) {
    size_t avail_size = 0;
    if(szBlock <= 0) return true;
    if(index >= page_table->count) return false;
    if(bFirst) {
        avail_size = find_last(&page_table->page_nodes[index].page, FREE)->size;
    }
    else {
        if(NULL == page_table->page_nodes[index].page.first_block) {
            avail_size = page_table->page_nodes[index].page.first_block->size;
        } else if(page_table->page_nodes[index].page.first_block->free &&
                  szBlock <= page_table->page_nodes[index].page.first_block->size) {
            return true;
        }
        else return false;
    }
    return found_multipage_space(page_table, index + 1, szBlock - avail_size, false);
}

u_int get_best_fit_offset(const_PagePtr restrict ptrPage, size_t szBlock, Block **best_fit) {
    Block *block = ptrPage->first_block;
    Block *best_block = NULL;
    size_t free_size = ptrPage->max_free_size;
    while(NULL != block) {
        if(block->size >= szBlock && block->size <= free_size && block->free) {
            free_size = block->size;
            best_block = block;
        }
        block = block->next;
    }
    *best_fit = best_block;
    return get_offset(ptrPage, best_block);
}

u_int get_offset(const_PagePtr ptrPage, const Block const *ptrBlock) {
    const Block *ptr = ptrPage->first_block;
    u_int offset = 0;
    while(NULL != ptr) {
        if(ptrBlock == ptr) break;
        offset += ptr->size;
        ptr = ptr->next;
    }
    return offset;
}

const Block *find_last(const_PagePtr restrict ptrPage, Block_type type) {
    const Block *ptr = ptrPage->first_block;
    const Block *last_block = ptrPage->first_block;
    if(type == ANY) {
        while(NULL != ptr->next) {
            last_block = ptr = ptr->next;
        }
    }
    else{
        while(NULL != ptr->next) {
            ptr = ptr->next;
            if(ptr->free == type)
                last_block = ptr;
        }
    }
    return last_block;
}

VA get_virtual_address(VA seg_addr, u_int numPage, size_t szPage, u_int szAddr, u_int offset) {
    u_int lg_szPage = log_2((u_int) szPage);
    unsigned long res = (unsigned long) seg_addr << log_2(szAddr << 1);
    res =  res << lg_szPage | numPage << lg_szPage | offset;
    return (VA) res;
}

u_int log_2(u_int num) {
    u_int res = 0;
    while(num >>=1) ++res;
    return res;
}

int insert_block(PageTablePtr restrict table, u_int j, Block *f_block, size_t szBlock, size_t szPage, int to_many) {
    Block *block = (Block *) malloc(sizeof(Block));
    if(NULL == block) return false;
    block->free = false;

    if(to_many) {
        f_block->free = false;
        szBlock -= f_block->size;
        set_max_free_size(&table->page_nodes[j].page);
        while(true) {
            ++j;
            if(szBlock < szPage) break;
            table->page_nodes[j].page.first_block->free = false;
            szBlock -= szPage;
            set_max_free_size(&table->page_nodes[j].page);
        }
        if(szBlock > 0) {
            block->next = table->page_nodes[++j].page.first_block;
            block->size = szBlock;
            block->next->size -= szBlock;
            if(block->next->size == 0) {
                Block *tmp = block->next;
                block->next = block->next->next;
                free(tmp);
            }
            table->page_nodes[++j].page.first_block = block;
        }
    }
    else {
        Block *prev;
        prev = find_prev(&table->page_nodes[j].page, f_block);
        block->size = szBlock;
        block->next = f_block;
        f_block->size -= szBlock;
        if(0 == f_block->size) free(f_block);
        set_max_free_size(&table->page_nodes[j].page);
        if(NULL == prev) table->page_nodes[j].page.first_block = block;
        else prev->next = block;
    }
    return true;
}

void set_max_free_size(PagePtr page) {
    Block *block = page->first_block;
    size_t max_size = 0;
    while(NULL != block) {
        if(block->free && max_size > block->size)
            max_size = block->size;
        block = block->next;
    }
    page->max_free_size = max_size;
}

Block *find_prev(const_PagePtr ptrPage, const Block const *f_block) {
    Block *ptr = ptrPage->first_block;

    while(NULL != ptr->next) {
        if (ptr->next == f_block)
            return ptr;
        ptr = ptr->next;
    }
    return NULL;
}





// TODO: delete later

void print_blocks(Block *first) {
    while(first) {
        printf("\t\t\t\taddr: %d, is_free: %d, size: %d\n", (int) first, first->free, (int) first->size);
        first = first->next;
    }
}

void print_all_what_I_need(SegmentTablePtr table) {
    printf("page size: %d, seg count: %d, mem size: %d\n", (int) table->szPage, table->count, table->szAddr);
    for(u_int i = 0; i < table->count; ++i) {
        printf("\t base_va: %d, base_pa: %d, size: %d, l|m|r: %d|%d|%d, rules: %d, ",
               (int) table->seg_nodes[i].descriptor.base_virt_addr,
               (int) table->seg_nodes[i].descriptor.base_phys_addr,
               (int) table->seg_nodes[i].descriptor.size,
               table->seg_nodes[i].descriptor.flags.loaded,
               table->seg_nodes[i].descriptor.flags.modified,
               table->seg_nodes[i].descriptor.flags.referenced,
               table->seg_nodes[i].descriptor.rules);
        printf("page_count: %d\n", table->seg_nodes->segment->count);
        for(u_int j = 0; j < table->seg_nodes[i].segment->count; ++j) {
            printf("\t\t base_pa: %d, l|m|r: %d|%d|%d, max_free_size: %d\n",
                   (int) table->seg_nodes[i].segment->page_nodes[j].descriptor.phys_addr,
                   table->seg_nodes[i].segment->page_nodes[j].descriptor.flags.loaded,
                   table->seg_nodes[i].segment->page_nodes[j].descriptor.flags.modified,
                   table->seg_nodes[i].segment->page_nodes[j].descriptor.flags.referenced,
                   (int) table->seg_nodes[i].segment->page_nodes[j].page.max_free_size
            );
            print_blocks(table->seg_nodes[i].segment->page_nodes[j].page.first_block);
        }
        printf("\n");
    }
    printf("\n\n\n");
}
