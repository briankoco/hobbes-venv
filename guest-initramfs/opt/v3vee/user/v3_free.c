/*
 * Copyright (c) 2011, Jack Lange <jacklange@cs.pitt.edu>
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
 
#include "v3_ioctl.h"
#include "v3vee.h"

int main(int argc, char* argv[]) {

    char * filename = argv[1];
    int ret = 0;

    if (argc <= 1) {
	printf("usage: v3_free <vm-device>\n");
	return -1;
    }



    printf("Freeing VM (%s)\n", filename);


    ret = v3_free_vm(get_vm_id_from_path(filename)); 

    if (ret < 0) {
        printf("Error freeing VM\n");
        return -1;
    }



    return 0; 
} 


