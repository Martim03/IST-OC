#include <string.h>
#include "SimpleCache.h"
#include "L2Cache.h"

uint8_t L1Cache[L1_SIZE];
uint8_t L2Cache[L2_SIZE];
CacheL1 SimpleCacheL1;
CacheL2 SimpleCacheL2;

void initCacheL2() {
    SimpleCacheL1.init = 0;
    SimpleCacheL2.init = 0;
}

void accessL1(uint32_t address, uint8_t *data, uint32_t mode) {

    uint32_t index, Tag;
    index = address & 0xFF0;
    index = index >> 4;
    uint8_t TempBlock[BLOCK_SIZE];

    /* init cache */
    if (SimpleCacheL1.init == 0) {
        for (int i = 0; i < L1_ENTRIES; i++) {
            SimpleCacheL1.line[i].Valid = 0;
        }
        SimpleCacheL1.init = 1;
    }

    CacheLine *Line = &SimpleCacheL1.line[index];

    Tag = address >> 12; // Why do I do this?

    if (!Line->Valid || Line->Tag != Tag) { // if block not present - miss
        accessL2(address, TempBlock, MODE_READ);

        if ((Line->Valid) && (Line->Dirty)) { // line has dirty block
            uint32_t OldAddress = Line->Tag << 8;
            OldAddress += index;
            OldAddress = OldAddress << 4;
            accessL2(OldAddress, &(L1Cache[index*BLOCK_SIZE]),MODE_WRITE);
        }

        memcpy(&(L1Cache[index*BLOCK_SIZE]), TempBlock,BLOCK_SIZE); // copy new block to cache line

        Line->Valid = 1;
        Line->Tag = Tag;
        Line->Dirty = 0;
    }

    ChangeL1(index, data, address, Line, mode, L1Cache);
}


// tag - 19 bits | index - 9 bits | offset - 4 bits
void accessL2(uint32_t address, uint8_t *data, uint32_t mode) {
    uint32_t index, Tag;
    uint8_t TempBlock[BLOCK_SIZE];

    index = address & 0x1FF0;
    index = index >> 4;

    if (SimpleCacheL2.init == 0) {
        for (int i = 0; i < L2_ENTRIES; i++) {
            SimpleCacheL2.line[i].Valid = 0;
        }
        SimpleCacheL2.init = 1;
    }

    CacheLine *Line = &SimpleCacheL2.line[index];

    Tag = address >> 13;

    if (!Line->Valid || Line->Tag != Tag) { // if block not present - miss
        uint32_t MemAddress = address >> 4;
        MemAddress = MemAddress << 4;
        accessDRAM(MemAddress, TempBlock, MODE_READ);

        if (Line->Valid && Line->Dirty) { // line has dirty block
                uint32_t OldMemAddress = Line->Tag << 9;
                OldMemAddress += index;
                OldMemAddress = OldMemAddress << 4;
                accessDRAM(OldMemAddress, &(L2Cache[index*BLOCK_SIZE]), MODE_WRITE);
        }

        memcpy(&(L2Cache[index*BLOCK_SIZE]), TempBlock, BLOCK_SIZE);

        Line->Valid = 1;
        Line->Tag = Tag;
        Line->Dirty = 0;
    }

    ChangeL2(index, data, Line, mode, L2Cache);
}