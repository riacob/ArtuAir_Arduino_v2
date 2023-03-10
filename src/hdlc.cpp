#include "hdlc.h"
#ifdef STDIODBG
#include <iostream>
#endif

HDLC::HDLC(uint8_t *workBuffer, int workBufferSize)
{
    workBuf = workBuffer;
    workBufSize = workBufferSize;
}

int HDLC::frame()
{
    int i = 0, j = 0, k = 0, m = 0;
    // Flag
    workBuf[0] = 0x7E;
    // Address
    workBuf[1] = data->ADD;
    // Control
    workBuf[2] = data->CTR;
    // Data
    for (i = 3; i < data->DATlen + 3; i++)
    {
        workBuf[i] = data->DAT[i - 3];
    }
    // CRC16
    uint16_t crc = CRC16::ccitt(workBuf + 1, 2 + data->DATlen);
    workBuf[i] = crc >> 8;
    workBuf[i + 1] = crc & 0x00FF;
    // Stuffing
    for (j = 1; j < i + 2; j++)
    {
        if (workBuf[j] < 0x1F || workBuf[j] == 0x7E || workBuf[j] == 0x7D)
        {
            workBuf[j] ^= 0x20;
            for (m = i + 3; m > 3; m--)
            {
                workBuf[m] = workBuf[m - 1];
            }
            workBuf[j] = 0x7D;
            k++;
        }
    }
    // Flag
    workBuf[i + 2 + k] = 0x7E;
#ifdef STDIODBG
    for (int l = 0; l < i + 2 + k + 1; l++)
    {
        std::cout << workBuf[l] << ",";
    }
    std::cout << std::endl
              << k << std::endl;
#endif
    // Return lenght of frame
    return i + k + 3;
}

bool HDLC::unframe()
{
    int i = 0, j = 0, m = 0;
    int framelgt = 0;
    bool validcrc = false;
    bool flgfound = false;
    // Get frame length
    do
    {
        framelgt++;
        if (workBuf[framelgt] == 0x7E)
        {
            framelgt++;
            flgfound = true;
            break;
        }
    } while (/*(workBuf[framelgt] != 0x7E) || */ (framelgt <= workBufSize));
    // If no flag was found return as if crc was invalid
    if (!flgfound)
    {
        return false;
    }
    // Unstuffing
    int newlen = framelgt;
    for (i = 1; i < framelgt - 1; i++)
    {
        if (workBuf[i] == 0x7D)
        {
            for (j = i; j < framelgt; j++)
            {
                workBuf[j] = workBuf[j + 1];
            }
            workBuf[i] ^= 0x20;
            newlen--;
        }
    }
    // CRC16 comparison
    Serial.println(framelgt);
    Serial.println(newlen);
    uint16_t crc = CRC16::ccitt(workBuf + 1, newlen - 4);
    Serial.println();
    for(int x = 0; x < newlen; x++) {
        Serial.print(workBuf[x]);
        Serial.print(",");
    }
    Serial.println();
    if ((workBuf[newlen - 2] == (crc >> 8)) && (workBuf[newlen - 3] == (crc & 0x00FF)))
    {
        validcrc = true;
        Serial.println(":)");
        Serial.print("crc1:");
        Serial.println(workBuf[newlen - 3]);
        Serial.print("crc2:");
        Serial.println(workBuf[newlen - 2]);
    }
    else
    {
        Serial.println(":(");
        Serial.print("crc1:");
        Serial.println(workBuf[newlen - 3]);
        Serial.print("crc2:");
        Serial.println(workBuf[newlen - 2]);
        return false;
    }
    // Address
    data->ADD = workBuf[1];
    // Control
    data->CTR = workBuf[2];
    // Data
    for (m = 3; m < framelgt - 6; m++)
    {
        data->DAT[m - 3] = workBuf[m];
    }
    // Data length
    data->DATlen = framelgt - 6;
#ifdef STDIODBG
    for (int l = 0; l < framelgt; l++)
    {
        std::cout << workBuf[l] << ",";
    }
    std::cout << std::endl
              << framelgt << std::endl;
    for (int k = 0; k < data->DATlen; k++)
    {
        std::cout << data->DAT[k] << ",";
    }
    std::cout << std::endl
              << data->DATlen << std::endl;
#endif
    return validcrc;
}

void HDLC::setData(HDLCData *hdlcdata)
{
    data = hdlcdata;
}

HDLC::HDLCData *HDLC::getData()
{
    return data;
}

uint8_t *HDLC::getWorkBuffer()
{
    return workBuf;
}

int HDLC::getWorkBufferSize()
{
    return workBufSize;
}