#ifndef SIMPLECACHE_H
#define SIMPLECACHE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "Cache.h"

void resetTime();

uint32_t getTime();

extern uint32_t time;

/****************  RAM memory (byte addressable) ***************/
void accessDRAM(uint32_t, uint8_t *, uint32_t);

/*********************** Cache *************************/

void initCache();

typedef struct CacheLine {
  uint8_t Valid;
  uint8_t Dirty;
  uint32_t Tag;
  int32_t AccessTime;
} CacheLine;

void ChangeL1(uint32_t index, uint8_t *data, uint32_t address, CacheLine *Line, uint32_t mode, uint8_t *cache);
void ChangeL2(uint32_t index, uint8_t *data, CacheLine *Line, uint32_t mode, uint8_t *cache);

/*********************** Interfaces *************************/

void read(uint32_t, uint8_t *);

void write(uint32_t, uint8_t *);

#endif
