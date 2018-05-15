/*
 * Copyright (c) 2015, Jack Lange <jacklange@cs.pitt.edu>
 * All rights reserved.
 *
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "V3VEE_LICENSE".
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h> 
#include <sys/ioctl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include <errno.h>
#include <sys/mman.h>
#include <linux/unistd.h>


#include <pet_ioctl.h>

#include "v3vee.h"
#include "v3_ioctl.h"
#include "v3_checkpoint.h"

#define PAGE_SIZE sysconf(_SC_PAGESIZE)


int 
v3_create_checkpoint(int vm_id)
{
    char * dev_path = get_vm_dev_path(vm_id);
    int    chkpt_fd = -1;

    chkpt_fd = pet_ioctl_path(dev_path, V3_VM_CREATE_CHECKPOINT, NULL);
    
    free(dev_path);

    return chkpt_fd;
}


ssize_t 
v3_chkpt_get_size(int chkpt_fd)
{
    return ioctl(chkpt_fd, V3_CHKPT_SIZE, 0);
}

struct chkpt_header * 
v3_chkpt_get(int chkpt_fd)
{
    unsigned int chkpt_size  = 0;
    unsigned int chkpt_pages = 0;

    chkpt_size = v3_chkpt_get_size(chkpt_fd);
    
    chkpt_pages = (chkpt_size / PAGE_SIZE) + ((chkpt_size % PAGE_SIZE) > 0);

    return (struct chkpt_header *)mmap(NULL, chkpt_pages * PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, chkpt_fd, 0);
}

int 
v3_chkpt_save(int chkpt_fd)
{
    return ioctl(chkpt_fd, V3_CHKPT_SAVE, 0);
}

int 
v3_chkpt_load(int chkpt_fd)
{
    return ioctl(chkpt_fd, V3_CHKPT_LOAD, 0);
}



/*

int main(int argc, char* argv[]) {
    struct chkpt_header * chkpt = NULL;
    int chkpt_fd = 0;

    if (argc < 2) {
	printf("usage: v3_checkpoint <vm_device>\n");
	return -1;
    }


    chkpt_fd   = v3_create_checkpoint(argv[1]);

    if (chkpt_fd == -1) {
	ERROR("Failed to create checkpoint\n");
	return -1;
    }

    chkpt = mmap_chkpt(chkpt_fd);

    if (chkpt == NULL) {
	ERROR("Could not map in checkpoint\n");
	return -1;
    }

    if (v3_chkpt_save(chkpt_fd);


    return 0; 
}


*/
