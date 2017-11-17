#ifndef MEMORY_MANAGER_PAGE_SEGMENT_H
#define MEMORY_MANAGER_PAGE_SEGMENT_H

#ifndef PHYS_ADDR
#define PHYS_ADDR
typedef char *PA;
#endif

#ifndef VIRT_ADDR
#define VIRT_ADDR
typedef char *VA;
#endif

typedef struct Block {
    struct Block*	next;
    size_t	        size;
    unsigned char   free: 1;
} Block;

typedef struct Page {
    Block   *first_block;
    size_t   max_free_size;
} Page;
typedef Page *PagePtr;
typedef const Page const *const_PagePtr;

typedef struct PageFlags {
    unsigned char   loaded: 1;
    unsigned char   referenced: 1;
    unsigned char   modified: 1;
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


typedef unsigned long size_t;

typedef enum AccessRules { READ = 1, WRITE = 2, EXEC = 4} AccessRules;

typedef struct SegmentFlags {
    unsigned char   loaded: 1;
    unsigned char   referenced: 1;
    unsigned char   modified: 1;
} SegmentFlags;

typedef PageTable Segment;

typedef PageTablePtr SegmentPtr;
typedef const PageTablePtr const *const_SegmentPtr;

typedef struct SegmentDescriptor {
    VA              base_virt_addr;
    PA              base_phys_addr;
    size_t          size;
    AccessRules     rules;
    SegmentFlags    flags;
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
    u_int           count;
    u_int           szAddr;
} SegmentTable;
typedef SegmentTable *SegmentTablePtr;
typedef const SegmentTable const *const_SegmentTablePtr;


int init_tables(SegmentTablePtr, u_int seg_count, u_int, size_t, PA);
void destroy_tables(SegmentTablePtr);
int alloc_block(SegmentTablePtr table, size_t szBlock, VA *va);
void print_all_what_I_need(SegmentTablePtr table);

#endif //MEMORY_MANAGER_PAGE_SEGMENT_H
