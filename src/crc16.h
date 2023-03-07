#ifndef CRC16_H
#define CRC16_H

#include <stdint.h>

class CRC16
{
public:
    static uint16_t ccitt(uint8_t *arr, int arrlgt);
};

#endif