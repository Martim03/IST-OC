#include <string.h>
#include "SimpleCache.h"
#include "L1Cache.h"

uint8_t Cache[L1_SIZE];
CacheL1 SimpleCache;

void initCacheL1() {
    SimpleCache.init = 0;
}

// tag - 20 bits | index - 8 bits | offset - 4 bits
void accessL1(uint32_t address, uint8_t *data, uint32_t mode) {
    uint32_t index, Tag;
    uint8_t TempBlock[BLOCK_SIZE];
    index = address & 0xFF0;
    index = index >> 4;

    /* init cache */
    if (SimpleCache.init == 0) {
        for (int i = 0; i < L1_ENTRIES; i++) {
            SimpleCache.line[i].Valid = 0;
        }
        SimpleCache.init = 1;
    }

    CacheLine *Line = &SimpleCache.line[index];

    Tag = address >> 12; // Why do I do this?

    /* access Cache*/
    if (!Line->Valid || Line->Tag != Tag) { // if block not present - miss
        uint32_t MemAddress = address >> 4;
        MemAddress = MemAddress << 4;
        accessDRAM(MemAddress, TempBlock, MODE_READ);

        if ((Line->Valid) && (Line->Dirty)) { // line has dirty block
            uint32_t OldAddress = Line->Tag << 8;
            OldAddress += index;
            OldAddress = OldAddress << 4;
            accessDRAM(OldAddress, &(Cache[index*BLOCK_SIZE]),MODE_WRITE);
        }

        memcpy(&(Cache[index*BLOCK_SIZE]), TempBlock,BLOCK_SIZE); // copy new block to cache line

        Line->Valid = 1;
        Line->Tag = Tag;
        Line->Dirty = 0;
    }

    ChangeL1(index, data, address, Line, mode, Cache);
}