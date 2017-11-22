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
                HardDrivePtr restrict drive) {

    MemBlock load_page;
    load_page.data = malloc(sizeof(char));
    load_page.page_va = get_va(table->szPage, table->szAddr, iSeg, iPage, 0);

    read_from_memory(table->seg_nodes[iSeg].segment->page_nodes[iPage].descriptor.phys_addr,
                     load_page.data,
                     table->szPage);

    memcpy(drive->data + drive->last * drive->szPage, &load_page.page_va, sizeof(VA));
    memcpy(drive->data + drive->last * drive->szPage + sizeof(VA),
           load_page.data, drive->szPage);
    drive->last++;
    table->seg_nodes[iSeg].segment->page_nodes[iPage].descriptor.phys_addr = NULL;

    table->seg_nodes[iSeg].segment->page_nodes[iPage].descriptor.flags.loaded = true;
    free(load_page.data);
    return SUCCESS;
}

int unload_page(SegmentTablePtr restrict table,
                const u_int iSeg,
                const u_int iPage,
                HardDrivePtr restrict drive) {

    MemBlock unload_page;
    VA unload_va = NULL;
    u_int i = 0;
    const u_int hd_entry_count = (const u_int) (drive->size / (drive->szPage + sizeof(VA)));

    unload_page.page_va = get_va(table->szPage, table->szAddr, iSeg, iPage, 0);
    for(; i < hd_entry_count; ++i) {
        memcpy(&unload_va, drive->data + i * drive->szPage, sizeof(VA));
        if(unload_va == unload_page.page_va) {
            memcpy(unload_page.data, drive->data + i * drive->szPage + sizeof(VA), drive->szPage);
            break;
        }
    }
    if(i == hd_entry_count) return EUNKNW;

    u_int numLoad_page = find_page_to_load(table->seg_nodes[iSeg].segment, iSeg);
    table->seg_nodes[iSeg].segment->page_nodes[iPage].descriptor.phys_addr =
            table->seg_nodes[iSeg].segment->page_nodes[numLoad_page].descriptor.phys_addr;
    write_to_memory(table->seg_nodes[iSeg].segment->page_nodes[iPage].descriptor.phys_addr,
                    unload_page.data,
                    drive->szPage);
    table->seg_nodes[iSeg].segment->page_nodes[iPage].descriptor.flags.loaded = false;
    load_page(table, iSeg, numLoad_page, drive);

    return SUCCESS;
}
