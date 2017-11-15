#ifndef MEMORY_MANAGER_PAGE_SEGMENT_H
#define MEMORY_MANAGER_PAGE_SEGMENT_H

typedef char *PA;

typedef struct Block {
    struct block*	next;
    unsigned int	size;
} Block;

typedef struct Page {
    Block *first_free;
    Block *first_used;
} Page;
typedef Page *PagePtr;

typedef struct PageFlags {
    unsigned char loaded: 1;
    unsigned char referenced: 1;
    unsigned char modified: 1;
} PageFlags;

typedef struct PageDescriptor {
    PA          phys_addr;
    PageFlags   flags;
} PageDescriptor;

typedef struct PageNode {
    PagePtr         page;
    PageDescriptor  descriptor;
} PageNode;
typedef PageNode *PageNodePtr;

typedef struct PageTable{
    PageNodePtr table;
    u_int count;
} PageTable;


typedef unsigned long size_t;

typedef enum AccessRules { READ = 1, WRITE = 2} AccessRules;

typedef struct SegmentFlags {;
    unsigned char   loaded: 1;
    unsigned char   referenced: 1;
    unsigned char   modified: 1;
} SegmentFlags;

typedef struct Segment {
    PageTable page_table;
} Segment;
typedef struct Segment *SegmentPtr;

typedef struct SegmentDescriptor {
    PA              base_addr;
    size_t          size;
    AccessRules     rules;
    SegmentFlags    flags;
} SegmentDescriptor;
typedef SegmentDescriptor *SegmentDescriptorPtr;

typedef struct SegmentNode {
    Segment segment;
    SegmentDescriptor descriptor;
} SegmentNode;
typedef SegmentNode *SegmentNodePtr;

typedef struct SegmentTable{
    SegmentNodePtr table;
    size_t szPage;
    u_int count;
} SegmentTable;
typedef SegmentTable *SegmentTablePtr;

int init_tables(SegmentTablePtr, u_int seg_count, u_int, size_t, PA);
void destroy_tables(SegmentTablePtr);

#endif //MEMORY_MANAGER_PAGE_SEGMENT_H
