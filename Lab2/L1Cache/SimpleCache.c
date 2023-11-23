#include <string.h>
#include "SimpleCache.h"
#include "L1Cache.h"

uint8_t DRAM[DRAM_SIZE];
uint32_t time = 0;

/**************** Time Manipulation ***************/
void resetTime() { time = 0; }

uint32_t getTime() { return time; }

/****************  RAM memory (byte addressable) ***************/
void accessDRAM(uint32_t address, uint8_t *data, uint32_t mode) {

    if (address >= DRAM_SIZE - WORD_SIZE + 1)
    exit(-1);

    if (mode == MODE_READ) {
    memcpy(data, &(DRAM[address]), BLOCK_SIZE);
    time += DRAM_READ_TIME;
    }

    if (mode == MODE_WRITE) {
    memcpy(&(DRAM[address]), data, BLOCK_SIZE);
    time += DRAM_WRITE_TIME;
    }
}

/*********************** L1 cache *************************/

void initCache() {
    initCacheL1();
}

void ChangeL1(uint32_t index, uint8_t *data, uint32_t address,
                  CacheLine *Line, uint32_t mode, uint8_t *cache) {

    uint32_t word_offset = address & 0xF;

    if (mode == MODE_READ) {
        memcpy(data, &(cache[index*BLOCK_SIZE + word_offset*WORD_SIZE]), WORD_SIZE);
    time += L1_READ_TIME;
    }

    if (mode == MODE_WRITE) { // write data from cache line
        memcpy(&(cache[index*BLOCK_SIZE + word_offset*WORD_SIZE]), data, WORD_SIZE);
    time += L1_WRITE_TIME;
    Line->Dirty = 1;
    }
}

void ChangeL2(uint32_t index, uint8_t *data, CacheLine *Line, uint32_t mode, uint8_t *cache) {
    if (mode == MODE_READ) {
        memcpy(data, &(cache[index*BLOCK_SIZE]), BLOCK_SIZE);
        time += L2_READ_TIME;
    }

    if (mode == MODE_WRITE) {
        memcpy(&(cache[index*BLOCK_SIZE]), data, BLOCK_SIZE);
        time += L2_WRITE_TIME;
        Line->Dirty = 1;
    }
}

void read(uint32_t address, uint8_t *data) {
    accessL1(address, data, MODE_READ);
}

void write(uint32_t address, uint8_t *data) {
    accessL1(address, data, MODE_WRITE);
}
