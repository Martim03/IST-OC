#include "2WayL2Cache.h"
#include "SimpleCache.h"
#include <math.h>

uint8_t L1Cache[L1_SIZE];
uint8_t L2Cache[L2_SIZE];
CacheL1 SimpleCacheL1;
CacheL2_2way CacheL2;


void initCacheL2_2way() {
    SimpleCacheL1.init = 0;
    CacheL2.init = 0;
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
        accessL2_2way(address, TempBlock, MODE_READ);

        if ((Line->Valid) && (Line->Dirty)) { // line has dirty block
            uint32_t OldAddress = Line->Tag << 8;
            OldAddress += index;
            OldAddress = OldAddress << 4;
            accessL2_2way(OldAddress, &(L1Cache[index*BLOCK_SIZE]),MODE_WRITE);
        }

        memcpy(&(L1Cache[index*BLOCK_SIZE]), TempBlock,BLOCK_SIZE); // copy new block to cache line

        Line->Valid = 1;
        Line->Tag = Tag;
        Line->Dirty = 0;
    }

    ChangeL1(index, data, address, Line, mode, L1Cache);
}


// tag - 20 bits | index - 8 bits | offset - 4 bits
// because of associativity
void accessL2_2way(uint32_t address, uint8_t *data, uint32_t mode) {
    uint32_t index, Tag, real_index;

    index = address & 0x0FF0;
    index = index >> 4;

    if (CacheL2.init == 0) {
        for (int i = 0; i < L2_2WAY_ENTRIES; i++) {
            for (int j = 0; j < ASSOCIATIVITY; j++) {
                CacheL2.cache_set[i].set_line[j].Valid = 0;
                CacheL2.cache_set[i].set_line[j].AccessTime = 0;
                CacheL2.cache_set[i].set_line[j].Dirty = 0;
                CacheL2.cache_set[i].set_line[j].Tag = 0;
            }
        }
        CacheL2.init = 1;
    }

    Tag = address >> 12;

    int hit = 0;
    int set_num = 0;
    for (int i = 0; i < ASSOCIATIVITY; i++) {
        if (CacheL2.cache_set[index].set_line[i].Valid && CacheL2.cache_set[index].set_line[i].Tag == Tag) {
            hit = 1;
            set_num = i;
            break;
        }
    }

    
    if (hit) {

        real_index = index + set_num * (int)pow(2, 8);
        
        /*
        for (int i = 0; i < L2_2WAY_ENTRIES; i++) {
            for (int j = 0; j < ASSOCIATIVITY; j++) {
                CacheL2.cache_set[i].set_line[j].AccessTime += 1;
            }
        }
        */

        ChangeL2(real_index, data, &(CacheL2.cache_set[index].set_line[set_num]), mode, L2Cache);

        CacheL2.cache_set[index].set_line[set_num].AccessTime = time;
    
    } else {

        uint8_t buffer_block[BLOCK_SIZE];
        uint32_t dram_address = address >> 4;
        dram_address = dram_address << 4;

        accessDRAM(dram_address, buffer_block, MODE_READ);

        int set_line = -1;
        for (int i = 0; i < ASSOCIATIVITY; i++) {
            if (!CacheL2.cache_set[index].set_line[i].Valid) {
                set_line = i;
                break;
            }
        }
        if (set_line == -1) {
            // find LRU with
            int32_t longest_time = CacheL2.cache_set[0].set_line[0].AccessTime;
            for (int i = 0; i < ASSOCIATIVITY; i++) {
                if (CacheL2.cache_set[index].set_line[i].AccessTime < longest_time) {
                    longest_time = CacheL2.cache_set[index].set_line[i].AccessTime;
                    set_line = i;
                }
            }
        }

        real_index = index + set_line * (int)pow(2, 8);

        if (CacheL2.cache_set[index].set_line[set_line].Dirty == 1) {

            uint32_t old_address_dram = CacheL2.cache_set[index].set_line[set_line].Tag << 8;
            old_address_dram += index;
            old_address_dram = old_address_dram << 4;
        
            accessDRAM(old_address_dram, &(L2Cache[real_index*BLOCK_SIZE]), MODE_WRITE);
        
        }

        memcpy(&(L2Cache[real_index*BLOCK_SIZE]), buffer_block, BLOCK_SIZE-1);

        CacheL2.cache_set[index].set_line[set_line].Tag = Tag;
        CacheL2.cache_set[index].set_line[set_line].Dirty = 0;
        CacheL2.cache_set[index].set_line[set_line].Valid = 1;
        
        ChangeL2(real_index, data, &(CacheL2.cache_set[index].set_line[set_line]), mode, L2Cache);

        CacheL2.cache_set[index].set_line[set_line].AccessTime = time;
    }

}