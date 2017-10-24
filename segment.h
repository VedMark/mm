#ifndef MEMORY_MANAGER_SEGMENT_H
#define MEMORY_MANAGER_SEGMENT_H

typedef unsigned long size_t;
typedef unsigned long addr;

typedef enum _seg_access_rules { READ = 1, WRITE = 2} ACCESSRULES;

struct _segment_flags {
    ACCESSRULES access;
    unsigned char loaded: 1;
    unsigned char referenced: 1;
    unsigned char modified: 1;
};
typedef struct _segment_flags SEGMENTFLAGS;
typedef unsigned long addr;

struct _segment{
    addr base_addr;
    size_t segment_size;
    SEGMENTFLAGS flags;
};
typedef struct _segment SEGMENT;
typedef struct SEGMENT *SEGMENTPTR;

struct _segment_node {
    SEGMENT segment;
    struct _segment_node *nextPtr;
};
typedef struct _segment_node SEGMENTNODE;
typedef SEGMENTNODE *SEGMENTNODEPTR;

struct _segment_table {
    SEGMENTNODEPTR head;
    SEGMENTNODEPTR tail;
};
typedef struct _segment_table SEGMENTTABLE;
typedef SEGMENTTABLE *SEGMENTTABLEPTR;

int segment_isEmpty(SEGMENTNODEPTR headPtr);
void segment_push(SEGMENTTABLEPTR, SEGMENT);
SEGMENT segment_pop(SEGMENTTABLEPTR);

#endif //MEMORY_MANAGER_SEGMENT_H
