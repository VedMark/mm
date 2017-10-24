#ifndef MEMORY_MANAGER_PAGE_H
#define MEMORY_MANAGER_PAGE_H


struct _page_flags {
    unsigned char loaded: 1;
    unsigned char referenced: 1;
    unsigned char modified: 1;
};
typedef struct _page_flags PAGEFLAGS;
typedef unsigned long addr;

struct _page {
    addr physical_page;
    PAGEFLAGS flags;
};
typedef struct _page PAGE;
typedef PAGE *PAGEPTR;

struct _pageNode {
    PAGE page;
    struct _pageNode *nextPtr;
};
typedef struct _pageNode PAGENODE;
typedef PAGENODE *PAGENODEPTR;

struct _page_table {
    PAGENODEPTR head;
    PAGENODEPTR tail;
};
typedef struct _page_table PAGETABLE;
typedef PAGETABLE *PAGETABLEPTR;

int page_isEmpty(PAGENODEPTR headPtr);
void page_push(PAGETABLEPTR, PAGE);
PAGE page_pop(PAGETABLEPTR);

#endif //MEMORY_MANAGER_PAGE_H
