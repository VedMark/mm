#include "include/hard_drive.h"

#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>

void init_hard_drive(HardDrivePtr drive, char *location, size_t size) {
    drive->fd = open(location, O_RDWR | O_CREAT | O_TRUNC);
    drive->location = location,
    drive->size = size;
    drive->data = (unsigned char *)mmap(NULL,
                                        size,
                                        PROT_READ | PROT_WRITE,
                                        MAP_SHARED,
                                        drive->fd,
                                        0);
}

void destroy_hard_drive(HardDrivePtr drive) {
    munmap(drive->data, drive->size);
    close(drive->fd);
    remove(drive->location);
}