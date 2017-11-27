#include <malloc.h>
#include <zconf.h>
#include <memory.h>

#include "include/page_segment.h"
#include "include/hard_drive.h"
#include "include/err_codes.h"
#include "include/cache.h"

typedef enum Block_type{USED = 0, FREE, ANY} Block_type;


const Block *find_last(const_PagePtr restrict ptrPage, Block_type type);
Block *find_prev(const_PagePtr ptrPage, const Block const *f_block);
bool found_multipage_space(const_PageTablePtr restrict page_table,
                           u_int index, size_t szBlock, int bFirst);
inline u_int log_2(u_int num);
u_int get_best_fit_offset(const_PagePtr restrict ptrPage,
                          size_t szBlock,
                          Block **best_fit);
inline u_int get_mask(u_int n);
u_int get_offset(const_PagePtr ptrPage, const Block const *ptrBlock);
bool insert_block(PageTablePtr restrict table,
                  u_int j,
                  Block *restrict f_block,
                  size_t szBlock,
                  size_t szPage,
                  int to_many);
void remove_block(PageTablePtr table, u_int iPage, Block *block, Block *prev);
void set_max_free_size(PagePtr page);


int alloc_block(SegmentTablePtr restrict table, size_t szBlock, VA *va) {
    u_int offset = 0;
    for(u_int i = 0; i < table->size; ++i) {
        SegmentPtr ptrSegment = table->seg_nodes[i].segment;
        for (u_int j = 0; j < ptrSegment->count; ++j) {
            if(0 == i && 0 == j) continue;
            if (szBlock > table->szPage) {
                if (found_multipage_space(ptrSegment, j, szBlock, true)) {
                    offset = get_offset(&ptrSegment->page_nodes[j].page,
                                        find_last(&ptrSegment->page_nodes[j].page, ANY));
                    *va = get_va(table->szPage, table->page_count, i, j, offset);
                    if(insert_block(table->seg_nodes[i].segment, j,
                                    (Block *) find_last(&ptrSegment->page_nodes[j].page, FREE),
                                    szBlock, table->szPage, true)) return SUCCESS;
                    else return EUNKNW;
                }
            }
            else {
                PagePtr ptrPage = &table->seg_nodes[i].segment->page_nodes[j].page;
                if (ptrPage->max_free_size >= szBlock) {
                    Block *best_block = NULL;
                    offset = get_best_fit_offset(ptrPage, szBlock, &best_block);
                    *va = get_va(table->szPage, table->page_count, i, j, offset);
                    if(insert_block(table->seg_nodes[i].segment,
                                    j, best_block, szBlock, table->szPage, false)) return SUCCESS;
                    else return EUNKNW;
                }
            }
        }
    }
    return EMLACK;
}

int free_block(SegmentTablePtr table, VA va){
    u_int n_seg = 0;
    u_int n_page = 0;
    u_int offset = 0;

    va_to_chunks(va, table->szPage, table->page_count, &n_seg, &n_page, &offset);
    if(0 == n_seg && 0 == n_page)
        return EUNKNW;
    Block *block = table->seg_nodes[n_seg].segment->page_nodes[n_page].page.first_block;
    Block *prev = NULL;

    u_int cur_offset = 0;
    while(NULL != block) {
        if(cur_offset >= offset) break;
        cur_offset += block->size;
        prev = block;
        block = block->next;
    }
    if(cur_offset != offset)
        return EUNKNW;

    remove_block(table->seg_nodes[n_seg].segment, n_page, block, prev);
    return SUCCESS;
}

int init_tables(SegmentTablePtr restrict table,
                const u_int seg_count,
                const u_int n,
                const size_t szPage,
                HardDrivePtr restrict drive,
                MemoryPtr restrict ram) {

    const u_int page_per_seg = n / seg_count;

    table->seg_nodes = (SegmentNodePtr) calloc(seg_count, sizeof(SegmentNode));
    if (NULL == table->seg_nodes) return EUNKNW;
    table->szPage = szPage;
    table->size = seg_count;
    table->page_count = n;

    for(u_int i = 0; i < seg_count; ++i) {
        table->seg_nodes[i].descriptor.base_virt_addr = (VA) (i * page_per_seg * szPage + 2);
        table->seg_nodes[i].descriptor.base_phys_addr = ram->field + i * page_per_seg * szPage;
        table->seg_nodes[i].descriptor.size = i * page_per_seg * szPage;

        table->seg_nodes[i].segment = (SegmentPtr) malloc(sizeof(Segment));
        table->seg_nodes[i].segment->count = page_per_seg;
        if(NULL == table->seg_nodes[i].segment) return EUNKNW;

        table->seg_nodes[i].segment->page_nodes = (PageNodePtr) calloc(n, sizeof(PageNode));
        if(NULL == table->seg_nodes[i].segment->page_nodes) return EUNKNW;

        for(u_int j = 0; j < page_per_seg; j++) {
            PA pa = table->seg_nodes[i].descriptor.base_phys_addr;
            PageNodePtr nodes = table->seg_nodes[i].segment->page_nodes;
            nodes[j].page.max_free_size = szPage;
            nodes[j].page.first_block = (Block *) malloc(sizeof(Block));
            if(NULL == nodes[j].page.first_block) return EUNKNW;

            nodes[j].page.first_block->next = NULL;
            nodes[j].page.first_block->size = szPage;
            nodes[j].page.first_block->free = true;
            nodes[j].page.first_block->splited = false;

            if(pa + j * szPage >= ram->field + ram->size) {
                nodes[j].descriptor.phys_addr = ram->field;
                if(EMLACK == push_page_to_drive(table, i, j, drive))
                    return EUNKNW;
                nodes[j].descriptor.flags.loaded = true;
            }
            else {
                nodes[j].descriptor.phys_addr = pa + j * szPage;
            }
        }
    }
}

void destroy_tables(SegmentTablePtr restrict table) {
    Block *ptr = NULL;
    if(NULL == table->seg_nodes) return;
    for(u_int i = 0; i < table->size; ++i) {
        for(u_int j = 0; j < table->seg_nodes[i].segment->count; j++) {
            ptr = table->seg_nodes[i].segment->page_nodes[j].page.first_block;
            while(NULL != ptr) {
                table->seg_nodes[i].segment->page_nodes[j].page.first_block = ptr->next;
                free(ptr);
                ptr = table->seg_nodes[i].segment->page_nodes[j].page.first_block;
            }
        }
        free(table->seg_nodes[i].segment->page_nodes);
        table->seg_nodes[i].segment->page_nodes = NULL;
        free(table->seg_nodes[i].segment);
        table->seg_nodes[i].segment = NULL;
    }
    free(table->seg_nodes);
    table->seg_nodes = NULL;
    table->size = 0;
    table->page_count = 0;
    table->szPage = 0;
}

bool validate_access(const_PageTablePtr restrict table,
                     const u_int iPage,
                     const u_int offset,
                     const size_t szBuffer) {

    const Block *block = find_block_by_offset(&table->page_nodes[iPage].page, offset);
    size_t diff = offset - get_offset(&table->page_nodes[iPage].page, block);
    size_t size = block->size;
    u_int n_page = iPage;
    while(block->splited) {
        block = table->page_nodes[++n_page].page.first_block;
        size += block->size;
    }
    return size - diff >= szBuffer;
}

void va_to_chunks(const_VA va, const size_t szPage, const u_int page_count,
                  u_int *iSeg, u_int *iPage, u_int *offset) {

    const u_int lg_szPage = log_2((u_int) szPage);
    const u_int lg_page_count = log_2(page_count);
    *offset = (u_int) ((unsigned long) va & get_mask(lg_szPage));
    *iPage = (u_int) ((unsigned long) va & get_mask(lg_page_count) << lg_szPage) >> lg_szPage;
    *iSeg = (u_int) ((unsigned long) va & get_mask(lg_page_count) << lg_page_count << lg_szPage)
            >> lg_page_count >> lg_szPage;
}

PA get_pa(const_SegmentTablePtr restrict table,
          const u_int iSeg,
          const u_int iPage,
          const u_int offset) {

    return table->seg_nodes[iSeg].segment->page_nodes[iPage].descriptor.flags.loaded ?
         NULL :
         table->seg_nodes[iSeg].segment->page_nodes[iPage].descriptor.phys_addr + offset;
}

void find_page_to_load(const_SegmentTablePtr restrict table, u_int *piSeg, u_int *piPage) {
    const Block *block1 = NULL;
    const Block *block2 = NULL;
    const_PageTablePtr pageTablePtr;
    for(u_int i = 0; i < table->size; ++i) {
        pageTablePtr = table->seg_nodes[i].segment;
        for (u_int j = 0; j < pageTablePtr->count; ++j) {
            if (!pageTablePtr->page_nodes[j].descriptor.flags.loaded &&
                (i || j)) {
                while (j < pageTablePtr->count) {
                    block2 = find_last(&pageTablePtr->page_nodes[j].page, ANY);
                    if (!block2->splited) {
                        if (j - 1 < 0) {
                            *piSeg=i, *piPage=j;
                            break;
                        }
                        block1 = find_last(&pageTablePtr->page_nodes[j - 1].page, ANY);
                        if (!(block1->splited)) {
                            *piSeg=i, *piPage=j;
                            break;
                        }
                    }
                    ++j;
                }
            }
        }
    }
}

void read_data(const_SegmentTablePtr restrict table,
               u_int iSeg,
               u_int iPage,
               u_int offset,
               void *pBuffer,
               size_t szBuffer) {

    PA pa = NULL;
    Block *block = NULL;
    size_t cur_shift = 0;
    size_t szRead = 0;
    size_t szRemain = szBuffer;

    while(cur_shift < szBuffer) {
        block = find_block_by_offset(&table->seg_nodes[iSeg].segment->page_nodes[iPage].page, offset);
        size_t sz = get_offset(&table->seg_nodes[iSeg].segment->page_nodes[iPage].page, block);
        pa = get_pa(table, iSeg, iPage, offset);
        szRead = block->size - (offset - sz);
        if(szRead > szRemain) szRead = szRemain;
        read_from_memory(pa, pBuffer + cur_shift, szRead);
        szRemain -=szRead;
        cur_shift += szRead;
        ++iPage;
        offset = 0;
    }
}

void write_data(const_SegmentTablePtr restrict table,
                u_int iSeg,
                u_int iPage,
                u_int offset,
                void *pBuffer,
                size_t szBuffer) {

    PA pa = NULL;
    Block *block = NULL;
    size_t cur_shift = 0;
    size_t szRead = 0;
    size_t szRemain = szBuffer;

    while(cur_shift < szBuffer) {
        block = find_block_by_offset(&table->seg_nodes[iSeg].segment->page_nodes[iPage].page, offset);
        size_t sz = get_offset(&table->seg_nodes[iSeg].segment->page_nodes[iPage].page, block);
        pa = get_pa(table, iSeg, iPage, offset);
        szRead = block->size - (offset - sz);
        if(szRead > szRemain) szRead = szRemain;
        write_to_memory(pa, pBuffer + cur_shift, szRead);
        szRemain -=szRead;
        cur_shift += szRead;
        ++iPage;
        offset = 0;
    }
}

size_t get_block_size(const_SegmentTablePtr table,
                     u_int iSeg,
                     u_int iPage,
                     Block *block) {

    size_t size = block->size;
    while(block->splited) {
        block = table->seg_nodes[iSeg].segment->page_nodes[iPage].page.first_block;
        size += block->size;
    }
    return size;
}

void read_block(const_SegmentTablePtr table,
                u_int iSeg,
                u_int iPage,
                u_int offset,
                CacheEntry *entry) {

    Block *block = find_block_by_offset(&table->seg_nodes[iSeg].segment->page_nodes[iPage].page, offset);
    offset = get_offset(&table->seg_nodes[iSeg].segment->page_nodes[iPage].page, block);
    PA pa = get_pa(table, iSeg, iPage, offset);
    entry->virt_addr = get_va(table->szPage, table->page_count, iSeg, iPage, offset);
    entry->block.data = malloc(get_block_size(table, iSeg, iPage, block) * sizeof(char));
    memcpy(entry->block.data, pa, block->size);
    entry->block.size = block->size;

    while(block->splited) {
        block = table->seg_nodes[iSeg].segment->page_nodes[++iPage].page.first_block;
        pa = get_pa(table, iSeg, iPage, 0);
        memcpy(entry->block.data + entry->block.size, pa, block->size);
        entry->block.size += block->size;
    }
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
        if(type != ptr->free) return NULL;
    }
    return last_block;
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

bool found_multipage_space(const_PageTablePtr restrict page_table,
                           u_int index,
                           size_t szBlock,
                           int bFirst) {
    size_t avail_size = 0;
    if(szBlock <= 0) return true;
    if(index >= page_table->count) return false;
    if(bFirst) {
        const Block *block = find_last(&page_table->page_nodes[index].page, FREE);
        if(NULL != block) avail_size = block->size;
        else {
            return false;
        }
    }
    else if(NULL == page_table->page_nodes[index].page.first_block->next) {
        if(szBlock >= page_table->page_nodes[index].page.first_block->size) {
            avail_size = page_table->page_nodes[index].page.first_block->size;
        }
        else {
            return true;
        }
    } else {
        return page_table->page_nodes[index].page.first_block->free &&
               szBlock <= page_table->page_nodes[index].page.first_block->size ? true : false;
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

Block *find_block_by_offset(const_PagePtr page, u_int offset) {
    if(offset == 0)
        return page->first_block;

    u_int curr_offset = 0;
    Block *block = page->first_block;
    do {
        curr_offset += block->size;
        if(curr_offset > offset) return block;
        block = block->next;
    } while(NULL != block);
    return block;
}

u_int get_mask(const u_int n) {
    const u_int src = (u_int) 1  << n;
    return src - 1;
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

VA get_va(const size_t szPage,
          const u_int page_count,
          const u_int iSeg,
          const u_int iPage,
          const u_int offset) {

    const u_int lg_szPage = log_2((u_int) szPage);
    const u_int lg_page_count = log_2(page_count);
    unsigned long res = iSeg << lg_page_count;
    res = res << lg_szPage | iPage << lg_szPage | offset;
    return (VA) res;
}

bool insert_block(PageTablePtr restrict table,
                  u_int j,
                  Block *f_block,
                  size_t szBlock,
                  size_t szPage,
                  int to_many) {

    Block *block = (Block *) malloc(sizeof(Block));
    if(NULL == block) return false;
    block->free = false;

    if(to_many) {
        f_block->free = false;
        f_block->splited = true;
        szBlock -= f_block->size;
        set_max_free_size(&table->page_nodes[j].page);
        while(true) {
            if(szBlock < szPage) break;
            ++j;
            table->page_nodes[j].page.first_block->free = false;
            szBlock -= szPage;
            if(szBlock > 0)
                table->page_nodes[j].page.first_block->splited = true;
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
            table->page_nodes[j].page.first_block = block;
            set_max_free_size(&table->page_nodes[j].page);
        }
    }
    else {
        Block *prev;
        prev = find_prev(&table->page_nodes[j].page, f_block);
        block->size = szBlock;
        f_block->size -= szBlock;
        if(0 == f_block->size) {
            free(f_block);
            block->next = NULL;
        }
        else block->next = f_block;
        set_max_free_size(&table->page_nodes[j].page);
        if(NULL == prev) table->page_nodes[j].page.first_block = block;
        else prev->next = block;
    }
    return true;
}

u_int log_2(u_int num) {
    u_int res = 0;
    while(num >>=1) ++res;
    return res;
}

void remove_block(PageTablePtr table, u_int iPage, Block *block, Block *prev) {
    if(block->splited){
        if (NULL != prev) {
            if (prev->free) {
                prev->next = NULL;
                prev->size += block->size;
                free(block);
                set_max_free_size(&table->page_nodes[iPage].page);
            }
            else {
                block->free = true;
                block->splited = false;
            }
            ++iPage;
        }
        while(iPage < table->count - 1 && table->page_nodes[iPage].page.first_block->splited) {
            table->page_nodes[iPage].page.first_block->free = true;
            table->page_nodes[iPage].page.first_block->splited = false;
            block = table->page_nodes[iPage + 1].page.first_block;
            if(NULL != block->next && block->next->free) {
                table->page_nodes[iPage + 1].page.first_block = block->next;
                block->next->size += block->size;
                free(block);
                set_max_free_size(&table->page_nodes[iPage].page);
            }
            else {
                block->free = true;
                block->splited = false;
                break;
            }
            ++iPage;
        }
    }
    else if(prev == NULL) {
        if(block->next && block->next->free) {
            table->page_nodes[iPage].page.first_block = block->next;
            block->next->size += block->size;
            free(block);
            set_max_free_size(&table->page_nodes[iPage].page);
        }
        else block->free = true;
    }
    else if(prev->free) {
        prev->next = block->next;
        prev->size += block->size;
        free(block);
        if(prev->next && prev->next->free) {
            block = prev->next;
            prev->next = block->next;
            prev->size += block->size;
            free(block);
            set_max_free_size(&table->page_nodes[iPage].page);
        }
    }
    else block->free = true;
}

void set_max_free_size(PagePtr page) {
    Block *block = page->first_block;
    size_t max_size = 0;
    while(NULL != block) {
        if(block->free && max_size < block->size)
            max_size = block->size;
        block = block->next;
    }
    page->max_free_size = max_size;
}





// TODO: delete later

void print_blocks(Block *first) {
    while(first) {
        printf("\t\t\t\taddr: %d, is_free: %d, splited: %d, size: %d\n", (int) first, first->free, first->splited, (int) first->size);
        first = first->next;
    }
}

void print_page_table(PageTablePtr table) {
    for(u_int j = 0; j < table->count; ++j) {
        printf("\t\t j: %d, base_pa: %d, l|m|r: %d|, max_free_size: %d\n", j,
               (int) table->page_nodes[j].descriptor.phys_addr,
               table->page_nodes[j].descriptor.flags.loaded,
               (int) table->page_nodes[j].page.max_free_size
        );
        print_blocks(table->page_nodes[j].page.first_block);
    }
}

void print_all_what_I_need(SegmentTablePtr table) {
    printf("page size: %d, seg size: %d, mem size: %d\n", (int) table->szPage, table->size, table->page_count);
    for(u_int i = 0; i < table->size; ++i) {
        printf("\t base_va: %d, base_pa: %d, size: %d, l|m|r: %d|, rules: %d, ",
               (int) table->seg_nodes[i].descriptor.base_virt_addr,
               (int) table->seg_nodes[i].descriptor.base_phys_addr,
               (int) table->seg_nodes[i].descriptor.size,
               table->seg_nodes[i].descriptor.flags.loaded,
               table->seg_nodes[i].descriptor.rules);
        printf("page_count: %d\n", table->seg_nodes->segment->count);
        print_page_table(table->seg_nodes[i].segment);
        printf("\n");
    }
    printf("\n\n\n");
}
