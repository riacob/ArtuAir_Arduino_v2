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
        uint8_t DAT[64];
        int DATlen;
    } HDLCData;

private:
    /**
     * @brief Pointer to the global serial buffer 
     */
    uint8_t *workBuf;
    /**
     * @brief Size of the global serial buffer 
     */
    int workBufSize;
    /**
     * @brief Pointer to a HDLCData struct, depending from the use it contains the received data of the data that will be sent
     */
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