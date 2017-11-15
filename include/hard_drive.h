#ifndef MEMOMY_MANAGER_HARD_DRIVE_H
#define MEMOMY_MANAGER_HARD_DRIVE_H

#include <stddef.h>

typedef struct FileMap{
    int fd;
    size_t size;
    char *location;
    unsigned char *data;
} FileMap;
typedef struct FileMap HardDrive;
typedef HardDrive *HardDrivePtr;

void init_hard_drive(HardDrivePtr drive, char *location, size_t size);
void destroy_hard_drive(HardDrivePtr drive);

#endif //MEMOMY_MANAGER_HARD_DRIVE_H
