#include "crc16.h"

uint16_t CRC16::ccitt(uint8_t *arr, int arrlgt)
{
    uint16_t temp;
    bool odd;
    uint16_t crc;
    int i, j;
    crc = 0xFFFF;
    for (i = 0; i < arrlgt; i++)
    {
        temp = arr[i] & 0x00FF;
        crc ^= temp;
        for (j = 0; j < 8; j++)
        {
            odd = crc & 0x0001 ? true : false;
            crc = crc >> 1;
            if (odd)
                crc ^= 0xA001;
        }
    }
    return crc;
}