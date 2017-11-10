#include "segment.h"
#include <stdlib.h>

//int segment_isEmpty(SegmentNodePtr headPtr) {
//    return headPtr == NULL;
//}
//
//void segment_push(SegmentTablePtr table, Segment value) {
//    SegmentNodePtr newPtr = malloc(sizeof(SegmentNode));
//
//    if(newPtr != NULL) {
//        *newPtr = (SegmentNode){
//                .segment.base_addr = value.base_addr,
//                .segment.segment_size = value.segment_size,
//                .nextPtr = NULL
//        };
//
//        if(segment_isEmpty(table->head))
//            table->head = newPtr;
//        else
//            table->tail->nextPtr = newPtr;
//
//        table->tail = newPtr;
//    }
//    else {
//        table->head = table->tail = newPtr;
//    }
//}
//
//Segment segment_pop(SegmentTablePtr table) {
//    Segment value;
//    SegmentNodePtr tempPtr;
//
//    value = table->head->segment;
//    tempPtr = table->head;
//    table->head = table->head->nextPtr;
//
//    if(table->head == NULL)
//        table->tail = NULL;
//
//    free(tempPtr);
//    return value;
//}
