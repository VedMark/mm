//#include "page.h"
//#include <stdlib.h>
//
//int table_isEmpty(PageTablePtr tablePtr) {
//    return tablePtr->head == NULL;
//}
//
//int table_push(PageTablePtr table, PagePtr value) {
//    PageNodePtr newPtr = malloc(sizeof(PageNode));
//
//    if(newPtr != NULL) {
//        PageDescriptor descr({0});
//
//        *newPtr = (PageNode){
//                .ptrPage = value,
//                .descriptor = descr,
//                .ptrPrev = table->tail->ptrPage,
//                .ptrNext = NULL
//        };
//
//        if(table_isEmpty(table)) table->head = newPtr;
//        table->tail = newPtr;
//    }
//    else return -2;
//    return 0;
//}
//
//Page table_pop(PageTablePtr table) {
//    Page value;
//    PageNodePtr tempPtr;
//
//    value = table->head->page;
//    tempPtr = table->head;
//    table->head = table->head->nextPtr;
//
//    if(table->head == NULL)
//        table->tail = NULL;
//
//    free(tempPtr);
//    return value;
//}
