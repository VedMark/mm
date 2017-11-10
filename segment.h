#ifndef MEMORY_MANAGER_SEGMENT_H
#define MEMORY_MANAGER_SEGMENT_H

#include "page.h"

typedef unsigned long size_t;

typedef enum AccessRules { READ = 1, WRITE = 2} AccessRules;

typedef struct SegmentFlags {
    AccessRules     access;
    unsigned char   loaded: 1;
    unsigned char   referenced: 1;
    unsigned char   modified: 1;
} SegmentFlags;

typedef struct Segment {
    PageTablePtr *pageTable;
} Segment;
typedef struct Segment *SegmentPtr;

typedef struct SegmentDescriptor {
    addr            baseAddr;
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

typedef SegmentNodePtr *SegmentTablePtr;

//
//int segment_isEmpty(SegmentNodePtr headPtr);
//void segment_push(SegmentTablePtr, Segment);
//Segment segment_pop(SegmentTablePtr);

#endif //MEMORY_MANAGER_SEGMENT_H
