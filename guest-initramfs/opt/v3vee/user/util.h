/*
 * Copyright (c) 2016, Jack Lange <jacklange@cs.pitt.edu>
 * All rights reserved.
 *
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "V3VEE_LICENSE".
 */


#ifndef __V3_UTIL_H__
#define __V3_UTIL_H__

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif


#define v3_free(ptr) {free(ptr); ptr = NULL;}

uint32_t v3_atou32(uint32_t dflt, char * str);
int32_t  v3_atoi32(int32_t  dflt, char * str);
uint64_t v3_atou64(uint64_t dflt, char * str);
int64_t  v3_atoi64(int64_t  dflt, char * str);


static inline int
v3_strlen(char * str) 
{
    if (str == NULL) {
        return -1;
    }

    return strlen(str);
}


#ifdef __cplusplus
}
#endif

#endif
