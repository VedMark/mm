#include "include/hard_drive.h"
#include "include/err_codes.h"

#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <malloc.h>
#include <memory.h>

void init_hard_drive(HardDrivePtr drive, char *location, size_t szPage, size_t size) {
    drive->fd = open(location, O_RDWR | O_CREAT);
    if(-1 == drive->fd) return;
    if(-1 == lseek(drive->fd, size-1, SEEK_SET)){
        drive->data = (void *) 1;
        return;
    }
    drive->last = 0;
    drive->location = location;
    drive->szPage = szPage;
    drive->size = size;
    drive->data = (unsigned char*) mmap(NULL,
                                        size,
                                        PROT_READ | PROT_WRITE,
                                        MAP_SHARED,
                                        drive->fd,
                                        0);
    if (1 != write(drive->fd, "", 1)) {
        destroy_hard_drive(drive);
        drive->fd = -1;
    }
}

void destroy_hard_drive(HardDrivePtr drive) {
    munmap(drive->data, drive->size);
    close(drive->fd);
    remove(drive->location);
}

int load_page(SegmentTablePtr restrict table,
              const u_int iSeg,
              const u_int iPage,
              size_t pos,
              HardDrivePtr restrict drive) {

    VA va = get_va(table->szPage, table->page_count, iSeg, iPage, 0);

    memcpy(drive->data + pos * drive->szPage, &va, sizeof(VA));
    void *ptr = malloc(drive->szPage);
    memcpy(ptr, table->seg_nodes[iSeg].segment->page_nodes[iPage].descriptor.phys_addr,
           drive->szPage);
    memcpy(drive->data + pos * drive->szPage + sizeof(VA),
           table->seg_nodes[iSeg].segment->page_nodes[iPage].descriptor.phys_addr,
           drive->szPage);

    table->seg_nodes[iSeg].segment->page_nodes[iPage].descriptor.phys_addr = NULL;
    table->seg_nodes[iSeg].segment->page_nodes[iPage].descriptor.flags.loaded = true;

    return SUCCESS;
}

int push_page_to_drive(SegmentTablePtr restrict table,
                       const u_int iSeg,
                       const u_int iPage,
                       HardDrivePtr restrict drive) {

    if((drive->last + 1) * drive->szPage > drive->size)
        return EMLACK;
    load_page(table, iSeg, iPage, drive->last, drive);
    drive->last++;

    return SUCCESS;
}

int unload_page(SegmentTablePtr restrict table,
                const u_int iSeg,
                const u_int iPage,
                HardDrivePtr restrict drive) {

    void *data = malloc(drive->szPage * sizeof(char));
    VA unload_va = NULL;
    u_int i = 0;
    const u_int hd_entry_count = (const u_int) (drive->size / (drive->szPage + sizeof(VA)));

    VA va = get_va(table->szPage, table->page_count, iSeg, iPage, 0);
    for(; i < hd_entry_count; ++i) {
        memcpy(&unload_va, drive->data + i * drive->szPage, sizeof(VA));
        if(unload_va == va) {
            memcpy(data, drive->data + i * drive->szPage + sizeof(VA), drive->szPage);
            break;
        }
    }
    if(i == hd_entry_count) return EUNKNW;

    u_int iLoad_seg = 0;
    u_int iLoad_page = 0;
    find_page_to_load(table, &iLoad_seg, &iLoad_page);

    table->seg_nodes[iSeg].segment->page_nodes[iPage].descriptor.flags.loaded = false;
    table->seg_nodes[iSeg].segment->page_nodes[iPage].descriptor.phys_addr =
            table->seg_nodes[iLoad_seg].segment->page_nodes[iLoad_page].descriptor.phys_addr;

    load_page(table, iLoad_seg, iLoad_page, i, drive);

    write_to_memory(table->seg_nodes[iSeg].segment->page_nodes[iPage].descriptor.phys_addr,
                    data,
                    drive->szPage);

    return SUCCESS;
}
