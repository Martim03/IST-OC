#include "SimpleCache.h"

int main() {

    uint32_t value4, value1, value2, value3, clock;

    resetTime();
    initCache();

    value1 = 10;
    value2 = 20;
    value3 = 30;

    /*write(41202, (uint8_t *)(&value1)); // 101 0  00001111  0010
    read(20722, (uint8_t *)(&value2)); // 010 1  00001111  0010
    read(41202, (uint8_t *)(&value3)); // 101 0  00001111  0010*/

    value1 = 10;
    value2 = 20;
    value3 = 30;
    value4 = 0;

    /*
    write(41202, (uint8_t *)(&value1)); // 101 0  00001111  0010
    read(20722, (uint8_t *)(&value2)); // 010 1  00001111  0010
    read(49394, (uint8_t *)(&value2)); // 110 0  00001111  0010
    read(41202, (uint8_t *)(&value3)); // 101 0  00001111  0010
    */

    write(1872, (uint8_t *)(&value1));
    write(848, (uint8_t *)(&value2));
    
    clock = getTime();
    printf("wrote first 2 shits: %d\n", clock);
    // ler primeiro, ou seja depois devia substituir o segundo valor
    read(1872, (uint8_t *)(&value4));

    clock = getTime();
    printf("read first value: %d\n", clock);
    write(1360, (uint8_t *)(&value3));

    clock = getTime();
    printf("wrote 3rd value: %d\n", clock);
    read(1872, (uint8_t *)(&value4));

    clock = getTime();
    printf("Read 1st value: %d\n", clock);

    printf("Value 4: %d, expected: %d\n", value4, 10);

    clock = getTime();
    printf("Time: %d\n", clock);

    read(1, (uint8_t *)(&value2));
    clock = getTime();
    printf("Time: %d\n", clock);

    write(512, (uint8_t *)(&value1));
    clock = getTime();
    printf("Time: %d\n", clock);

    read(512, (uint8_t *)(&value2));
    clock = getTime();
    printf("Time: %d\n", clock);

    return 0;
}