#ifndef OCLAB_2WAYL2CACHE_H
#define OCLAB_2WAYL2CACHE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "Cache.h"
#include "L1Cache.h"
#include "SimpleCache.h"

#define ASSOCIATIVITY 2
#define L2_2WAY_ENTRIES (L2_SIZE / BLOCK_SIZE / ASSOCIATIVITY)

void initCacheL2_2way();

typedef struct CacheSet {
    CacheLine set_line[ASSOCIATIVITY];
} CacheSet;

typedef struct CacheL2_2way {
    uint32_t init;
    CacheSet cache_set[L2_2WAY_ENTRIES];
} CacheL2_2way;

void accessL1(uint32_t address, uint8_t *data, uint32_t mode);

void accessL2_2way();

#endif //OCLAB_2WAYL2CACHE_H
