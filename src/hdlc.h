#ifndef HDLC_H
#define HDLC_H

#include <stdint.h>
#include <Arduino.h>
#define STDIODBG_

class HDLC
{
public:
    typedef struct
    {
        uint8_t ADD;
        uint8_t CTR;
        uint8_t *DAT;
        int DATlen;
    } HDLCData;

private:
    uint8_t *workBuf;
    int workBufSize;
    HDLCData *data;

public:
    HDLC(uint8_t *workBuffer, int workBufferSize);
    ~HDLC();
    int frame();
    bool unframe();
    void setData(HDLCData *hdlcdata);
    HDLCData *getData();
    uint8_t *getWorkBuffer();
    int getWorkBufferSize();
    uint16_t crc16_ccitt(uint8_t *arr, uint32_t arrlgt);
};

#endif