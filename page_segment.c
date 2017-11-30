#include <malloc.h>
#include <zconf.h>
#include <memory.h>

#include "include/page_segment.h"
#include "include/hard_drive.h"
#include "include/err_codes.h"

typedef enum Block_type{USED = 0, FREE, ANY} Block_type;


const Block *find_last(const_PagePtr restrict ptrPage, Block_type type);
bool found_multipage_space(const_PageTablePtr restrict page_table,
                           u_int index, size_t szBlock, int bFirst);
inline u_int log_2(u_int num);
size_t get_best_fit_offset(const_PagePtr restrict ptrPage,
                           size_t szBlock,
                           Block **best_fit);
inline u_int get_mask(u_int n);
size_t get_offset(const_PagePtr ptrPage, const Block const *ptrBlock);

bool insert_block(PageTablePtr restrict table, u_int j, Block *f_block, size_t szBlock, size_t szPage);
void remove_block(PageTablePtr table, u_int iPage, Block *block, Block *prev);
void set_max_free_size(PagePtr page);


int alloc_block(SegmentTablePtr restrict table, size_t szBlock, VA *va) {
    SegmentPtr ptrSegment = NULL;
    size_t offset = 0;

    for(u_int i = 0; i < table->size; ++i) {
        ptrSegment = table->seg_nodes[i].segment;
        for (u_int j = 0; j < ptrSegment->count; ++j) {
            if(0 == i && 0 == j) continue;
            if (szBlock > table->szPage) {
                if (found_multipage_space(ptrSegment, j, szBlock, true)) {
                    offset = get_offset(&ptrSegment->page_nodes[j].page,
                                        find_last(&ptrSegment->page_nodes[j].page, ANY));
                    *va = get_va(table->szPage, table->page_count, i, j, offset);
                    if(insert_block(table->seg_nodes[i].segment,
                                    j,
                                    (Block *) find_last(&ptrSegment->page_nodes[j].page, FREE),
                                    szBlock,
                                    table->szPage)) return SUCCESS;
                    else return EUNKNW;
                }
            }
            else {
                PagePtr ptrPage = &table->seg_nodes[i].segment->page_nodes[j].page;
                if (ptrPage->max_free_size >= szBlock) {
                    Block *best_block = NULL;
                    offset = get_best_fit_offset(ptrPage, szBlock, &best_block);
                    *va = get_va(table->szPage, table->page_count, i, j, offset);
                    if(insert_block(table->seg_nodes[i].segment, j,
                                    best_block,
                                    szBlock,
                                    table->szPage)) return SUCCESS;
                    else return EUNKNW;
                }
            }
        }
    }
    return EMLACK;
}

int free_block(SegmentTablePtr table, VA va){
    u_int iSeg = 0;
    u_int iPage = 0;
    u_int offset = 0;

    va_to_chunks(va, table->szPage, table->page_count, &iSeg, &iPage, &offset);
    if(!(iSeg || iPage) || iSeg >= table->size ||
       iPage >= table->seg_nodes[iSeg].segment->count){
        return EUNKNW;
    }
    Block *block = table->seg_nodes[iSeg].segment->page_nodes[iPage].page.first_block;
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

    if(block)
        remove_block(table->seg_nodes[iSeg].segment, iPage, block, prev);
    return SUCCESS;
}

int init_tables(SegmentTablePtr restrict table,
                const u_int seg_count,
                const u_int n,
                const size_t szPage,
                HardDrivePtr restrict drive,
                MemoryPtr restrict ram) {

    const u_int page_per_seg = n / seg_count;

    table->seg_nodes = (SegmentNodePtr) malloc(seg_count * sizeof(SegmentNode));
    if (NULL == table->seg_nodes) return EUNKNW;
    table->szPage = szPage;
    table->size = seg_count;
    table->page_count = n;

    for(u_int i = 0; i < seg_count; ++i) {
        table->seg_nodes[i].descriptor.base_phys_addr = ram->field + i * page_per_seg * szPage;

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
            if(NULL == nodes[j].page.first_block)
                return EUNKNW;

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
          size_t offset) {

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
            if (!pageTablePtr->page_nodes[j].descriptor.flags.loaded && (i || j)) {
                block2 = find_last(&pageTablePtr->page_nodes[j].page, ANY);
                if (block2 && !block2->splited) {
                    if (j <= 0) {
                        *piSeg = i, *piPage = j;
                        return;
                    }
                    block1 = find_last(&pageTablePtr->page_nodes[j - 1].page, ANY);
                    if (block1 && !block1->splited) {
                        *piSeg = i, *piPage = j;
                        return;
                    }
                }
            }
        }
    }
    for(u_int i = 0; i < table->size; ++i) {
        pageTablePtr = table->seg_nodes[i].segment;
        for (u_int j = 0; j < pageTablePtr->count; ++j) {
            if (!pageTablePtr->page_nodes[j].descriptor.flags.loaded && (i || j)) {
                *piSeg=i, *piPage=j;
                return;
            }
        }
    }
}

void read_data(SegmentTablePtr table,
               u_int iSeg,
               u_int iPage,
               u_int offset,
               void *pBuffer,
               size_t szBuffer,
               HardDrivePtr drive) {

    PA pa = NULL;
    Block *block = NULL;
    size_t cur_shift = 0;
    size_t szRead = 0;
    size_t szRemain = szBuffer;

    while(cur_shift < szBuffer) {
        block = find_block_by_offset(&table->seg_nodes[iSeg].segment->page_nodes[iPage].page, offset);
        size_t sz = get_offset(&table->seg_nodes[iSeg].segment->page_nodes[iPage].page, block);
        if(table->seg_nodes[iSeg].segment->page_nodes[iPage].descriptor.flags.loaded)  {
            if(EUNKNW == unload_page(table, iSeg, iPage, drive)) {
                return;
            }
        }
        pa = get_pa(table, iSeg, iPage, offset);
        if(NULL == pa) return;

        szRead = block->size - (offset - sz);
        if(szRead > szRemain) szRead = szRemain;
        read_from_memory(pa, pBuffer + cur_shift, szRead);
        szRemain -=szRead;
        cur_shift += szRead;
        ++iPage;
        offset = 0;
    }
}

void write_data(SegmentTablePtr table,
                u_int iSeg,
                u_int iPage,
                u_int offset,
                void *pBuffer,
                size_t szBuffer,
                HardDrivePtr drive) {

    PA pa = NULL;
    Block *block = NULL;
    size_t cur_shift = 0;
    size_t szRead = 0;
    size_t szRemain = szBuffer;

    while(cur_shift < szBuffer) {
        block = find_block_by_offset(&table->seg_nodes[iSeg].segment->page_nodes[iPage].page, offset);
        size_t sz = get_offset(&table->seg_nodes[iSeg].segment->page_nodes[iPage].page, block);
        if(table->seg_nodes[iSeg].segment->page_nodes[iPage].descriptor.flags.loaded)  {
            if(EUNKNW == unload_page(table, iSeg, iPage, drive)) {
                return;
            }
        }
        pa = get_pa(table, iSeg, iPage, offset);
        if(NULL == pa) return;

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
        block = table->seg_nodes[iSeg].segment->page_nodes[++iPage].page.first_block;
        size += block->size;
    }
    return size;
}

CacheEntryPtr read_block(SegmentTablePtr table, u_int iSeg, u_int iPage, size_t offset, HardDrivePtr drive) {
    CacheEntry *entry;
    Block *block = find_block_by_offset(&table->seg_nodes[iSeg].segment->page_nodes[iPage].page, offset);
    PA pa = NULL;

    offset = get_offset(&table->seg_nodes[iSeg].segment->page_nodes[iPage].page, block);
    if(table->seg_nodes[iSeg].segment->page_nodes[iPage].descriptor.flags.loaded) {
        if(EUNKNW == unload_page(table, iSeg, iPage, drive)) {
            return NULL;
        }
    }
    pa = get_pa(table, iSeg, iPage, offset);
    entry= malloc(sizeof(CacheEntry));
    entry->virt_addr = get_va(table->szPage, table->page_count, iSeg, iPage, offset);
    entry->block.data = malloc(get_block_size(table, iSeg, iPage, block) * sizeof(char));
    memcpy(entry->block.data, pa, block->size);
    entry->block.size = block->size;

    while(block->splited) {
        block = table->seg_nodes[iSeg].segment->page_nodes[++iPage].page.first_block;
        if(table->seg_nodes[iSeg].segment->page_nodes[iPage].descriptor.flags.loaded) {
            if(EUNKNW == unload_page(table, iSeg, iPage, drive)) {
                free(entry->block.data);
                free(entry);
                return NULL;
            }
        }
        pa = get_pa(table, iSeg, iPage, 0);
        if(NULL == pa) {
            free(entry->block.data);
            free(entry);
            return NULL;
        }
        memcpy(entry->block.data + entry->block.size, pa, block->size);
        entry->block.size += block->size;
    }
    return entry;
}

const Block *find_last(const_PagePtr restrict ptrPage, Block_type type) {
    const Block *ptr = ptrPage->first_block;
    const Block *last_block = ptrPage->first_block;
    if(NULL == ptr) return NULL;
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

size_t get_best_fit_offset(const_PagePtr restrict ptrPage, size_t szBlock, Block **best_fit) {
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
    return get_offset(ptrPage, best_block) + best_block->size - szBlock;
}

Block *find_block_by_offset(const_PagePtr page, size_t offset) {
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

size_t get_offset(const_PagePtr ptrPage, const Block const *ptrBlock) {
    const Block *ptr = ptrPage->first_block;
    size_t offset = 0;
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
          size_t offset) {

    const u_int lg_szPage = log_2((u_int) szPage);
    const u_int lg_page_count = log_2(page_count);
    unsigned long res = iSeg << lg_page_count;
    res = res << lg_szPage | iPage << lg_szPage | offset;
    return (VA) res;
}

bool insert_block(PageTablePtr restrict table, u_int j, Block *f_block, size_t szBlock, size_t szPage) {
    if(szBlock > szPage) {
        f_block->free = false;
        f_block->splited = true;
        szBlock -= f_block->size;
        set_max_free_size(&table->page_nodes[j].page);
        while(szBlock >= szPage) {
            ++j;
            table->page_nodes[j].page.first_block->free = false;
            szBlock -= szPage;
            if(szBlock > 0)
                table->page_nodes[j].page.first_block->splited = true;
            set_max_free_size(&table->page_nodes[j].page);
        }
        if(szBlock > 0) {
            Block *tmp = table->page_nodes[++j].page.first_block;
            if(szBlock == tmp->size) {
                tmp->free = false;
            }
            else {
                Block *new_block = (Block *) malloc(sizeof(Block));
                new_block->next = tmp;
                new_block->size = szBlock;
                new_block->free = false;
                new_block->splited = false;
                tmp->size -= szBlock;
                table->page_nodes[j].page.first_block = new_block;
            }
            set_max_free_size(&table->page_nodes[j].page);
        }
    }
    else {
        if(f_block->size == szBlock) {
            f_block->free = false;
            f_block->splited = false;
        }
        else {
            Block *block = (Block *) malloc(sizeof(Block));
            if(NULL == block) return false;
            block->next = f_block->next;
            block->free = false;
            block->splited = false;
            block->size = szBlock;
            f_block->size -= szBlock;
            f_block->next = block;
        }
        set_max_free_size(&table->page_nodes[j].page);
    }
    return true;
}

u_int log_2(u_int num) {
    u_int res = 0;
    while(num >>=1) ++res;
    return res;
}

void remove_block(PageTablePtr table, u_int iPage, Block *block, Block *prev) {
    if(block->splited) {
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
