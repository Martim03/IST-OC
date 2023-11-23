#ifndef OCLAB_L2CACHE_H
#define OCLAB_L2CACHE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "Cache.h"
#include "L1Cache.h"
#include "SimpleCache.h"
#define L2_ENTRIES (L2_SIZE/BLOCK_SIZE)

void initCacheL2();

typedef struct CacheL2 {
    uint32_t init;
    CacheLine line[L2_ENTRIES];
} CacheL2;

void accessL2(uint32_t, uint8_t *, uint32_t);

#endif //OCLAB_L2CACHE_H
