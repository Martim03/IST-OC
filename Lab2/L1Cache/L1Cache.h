#ifndef OCLAB_L1CACHE_H
#define OCLAB_L1CACHE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "Cache.h"
#include "SimpleCache.h"
#define L1_ENTRIES (L1_SIZE/BLOCK_SIZE)

void initCacheL1();

typedef struct CacheL1 {
    uint32_t init;
    CacheLine line[L1_ENTRIES];
} CacheL1;

void accessL1(uint32_t, uint8_t *, uint32_t);

#endif //OCLAB_L1CACHE_H
