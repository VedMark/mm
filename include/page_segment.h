#ifndef MM_PAGE_SEGMENT_H
#define MM_PAGE_SEGMENT_H

#include <stdbool.h>

#include "ram.h"
#include "cache.h"

typedef struct CacheEntry CacheEntry;
typedef struct FileMap HardDrive;

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

typedef struct Block {
    struct Block*	next;
    size_t	        size;
    unsigned char   free: 1;
    unsigned char   splited: 1;
} Block;

typedef struct Page {
    Block   *first_block;
    size_t   max_free_size;
} Page;
typedef Page *PagePtr;
typedef const Page const *const_PagePtr;

typedef struct PageFlags {
    unsigned char   loaded: 1;
} PageFlags;

typedef struct PageDescriptor {
    PA          phys_addr;
    PageFlags   flags;
} PageDescriptor;

typedef struct PageNode {
    Page            page;
    PageDescriptor  descriptor;
} PageNode;
typedef PageNode *PageNodePtr;
typedef const PageNode const *const_PageNodePtr;

typedef struct PageTable{
    PageNodePtr     page_nodes;
    u_int           count;
} PageTable;
typedef PageTable *PageTablePtr;
typedef const PageTable const*const_PageTablePtr;



typedef enum AccessRules { READ = 1, WRITE = 2, EXEC = 4} AccessRules;

typedef PageTable Segment;

typedef PageTablePtr SegmentPtr;
typedef const PageTablePtr const *const_SegmentPtr;

typedef struct SegmentDescriptor {
    PA              base_phys_addr;
    AccessRules     rules;
} SegmentDescriptor;
typedef SegmentDescriptor *SegmentDescriptorPtr;

typedef struct SegmentNode {
    SegmentPtr          segment;
    SegmentDescriptor   descriptor;
} SegmentNode;
typedef SegmentNode *SegmentNodePtr;
typedef const SegmentNode const *const_SegmentNodePtr;

typedef struct SegmentTable{
    SegmentNodePtr  seg_nodes;
    size_t          szPage;
    u_int           size;
    u_int           page_count;
} SegmentTable;
typedef SegmentTable *SegmentTablePtr;
typedef const SegmentTable const *const_SegmentTablePtr;


int alloc_block(SegmentTablePtr table, size_t szBlock, VA *va);
int free_block(SegmentTablePtr table, VA va);
int init_tables(SegmentTablePtr table,
                u_int seg_count,
                u_int n,
                size_t szPage,
                HardDrive *drive,
                MemoryPtr ram);
void destroy_tables(SegmentTablePtr table);
bool validate_access(const_PageTablePtr table,
                     u_int numPage,
                     u_int offset,
                     size_t szBuffer);
void va_to_chunks(const_VA va,
                  size_t szPage,
                  u_int szAddr,
                  u_int *numSeg,
                  u_int *numPage,
                  u_int *offset);
VA get_va(size_t szPage, u_int szAddr, u_int iSeg, u_int iPage, size_t offset);
PA get_pa(const_SegmentTablePtr table, u_int numSeg, u_int numPage, size_t offset);
void find_page_to_load(const_SegmentTablePtr restrict pageTablePtr, u_int *piSeg, u_int *piPage);
Block *find_block_by_offset(const_PagePtr page, size_t offset);
void read_data(SegmentTablePtr table,
               u_int iSeg,
               u_int iPage,
               u_int offset,
               void *pBuffer,
               size_t szBuffer,
               HardDrive *drive);
void write_data(SegmentTablePtr table,
                u_int iSeg,
                u_int iPage,
                u_int offset,
                void *pBuffer,
                size_t szBuffer,
                HardDrive *drive);
CacheEntry *read_block(SegmentTablePtr table, u_int iSeg, u_int iPage, size_t offset, HardDrive *drive);

#endif //MM_PAGE_SEGMENT_H
