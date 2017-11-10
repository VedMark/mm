#ifndef MEMORY_MANAGER_PAGE_H
#define MEMORY_MANAGER_PAGE_H

typedef unsigned long long addr;

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
    addr        phys_addr;
    PageFlags   flags;
} PageDescriptor;

typedef struct PageNode {
    PagePtr         page;
    PageDescriptor  descriptor;
} PageNode;
typedef PageNode *PageNodePtr;

typedef PageNodePtr *PageTablePtr;

#endif //MEMORY_MANAGER_PAGE_H
