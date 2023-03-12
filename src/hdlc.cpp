#include "hdlc.h"
#ifdef STDIODBG
#include <iostream>
#endif

HDLC::HDLC(uint8_t *workBuffer, int workBufferSize)
{
    // Get the pointer and the size of the global serial buffer
    workBuf = workBuffer;
    workBufSize = workBufferSize;
    /**
     * @brief Internal HDLC data container
     * 
     */
    data = new HDLCData;
}

HDLC::~HDLC(){
    delete data;
}

/**
 * @brief Prepares the serial buffer framing the data 
 * 
 * @return Length of the serial buffer 
 */
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
    // The counter starts at 3 because the first 3 idxs alredy contain data
    for (i = 3; i < data->DATlen + 3; i++)
    {
        workBuf[i] = data->DAT[i - 3];
    }

    // CRC16
    // Calculate the CRC16
    /**
     * @brief The calculated CRC16
     */
    uint16_t crc = crc16_ccitt(workBuf + 1, 2 + data->DATlen);
    // CRC16 MSB
    workBuf[i] = crc >> 8;
    // CRC16 LSB
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
    // Return length of frame
    return i + k + 3;
}

/**
 * @brief Unframes the serial buffer and check if the CRC16 is valid
 * 
 * @return true The CRC16 is valid
 * @return false No flag found or invalid CRC16
 */
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
        Serial.print(workBuf[framelgt]);
        Serial.print(",");
        // If a flag is found it gets out of the loop as there is no more data
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

    /**
     * @brief The length of the unstuffed buffer
     */
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

    delayMicroseconds(500);
    
    // Address
    data->ADD = workBuf[1];
    // Control
    data->CTR = workBuf[2];
    // Data
    for (m = 3; m < newlen - 3; m++)
    {
        data->DAT[m - 3] = workBuf[m];
    }

    // Data length (Size of the serial buffer - 6 (2 Flag + Add + Ctr + 2 CRC))
    data->DATlen = newlen - 6;
    uint16_t crc = crc16_ccitt(workBuf + 1, newlen - 4);

    // CRC16 comparison
    if ((workBuf[newlen - 2] == (crc >> 8)) && (workBuf[newlen - 3] == (crc & 0x00FF)))
    {
        validcrc = true;
        Serial.println(":)");
    }
    else
    {
        Serial.println(":(");
        return false;
    }

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

/**
 * @brief Changes the pointer to the pointer of the 
 * 
 * @param hdlcdata 
 */
void HDLC::setData(HDLCData *hdlcdata)
{
    data = hdlcdata;
}

/**
 * @brief Returns a pointer to the internal HDLCData struct
 * S
 * @return HDLC::HDLCData* 
 */
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

/**
 * @brief Calculate the CRC16 of a given data array
 * 
 * @param arr The array containing the data from which the CRC16 will be calculated
 * @param arrlgt The length of the array
 * @return uint16_t The CRC16
 */
uint16_t HDLC::crc16_ccitt(uint8_t *arr, uint32_t arrlgt)
{
    uint16_t temp;
    bool odd;
    uint16_t crc;
    uint32_t i, j;
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