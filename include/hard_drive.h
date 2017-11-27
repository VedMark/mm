#ifndef MM_HARD_DRIVE_H
#define MM_HARD_DRIVE_H

#include <stddef.h>
#include <zconf.h>

#include "page_segment.h"

typedef struct FileMap{
    int fd;
    size_t last;
    size_t szPage;
    size_t size;
    char *location;
    unsigned char *data;
} FileMap;
typedef struct FileMap HardDrive;
typedef HardDrive *HardDrivePtr;


void init_hard_drive(HardDrivePtr drive, char *location, size_t szPage, size_t size);
void destroy_hard_drive(HardDrivePtr drive);
int push_page_to_drive(SegmentTablePtr table,
                       u_int iSeg,
                       u_int iPage,
                       HardDrivePtr restrict drive);
int unload_page(SegmentTablePtr table,
                u_int iSeg,
                u_int iPage,
                HardDrivePtr restrict drive);

#endif //MM_HARD_DRIVE_H
