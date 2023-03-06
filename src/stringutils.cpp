/**
 * @file stringutils.cpp
 * @authors 5ELIA Malignani Udine A.S. 2022/2023
 * @brief
 * @version 0.1
 * @date 2023-03-06
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "stringutils.h"

void StringUtils::dateToString(uint8_t day, uint8_t month, uint16_t year, char *strbuf)
{
    // Convert date from numbers to a string (dd/mm/yyyy)
    // 48 decimal is ascii 0
    strbuf[0] = (day / 10) + 48;
    strbuf[1] = (day % 10) + 48;
    strbuf[2] = '/';
    strbuf[3] = (month / 10) + 48;
    strbuf[4] = (month % 10) + 48;
    strbuf[5] = '/';
    strbuf[6] = (year / 1000) + 48;
    strbuf[7] = ((year % 1000) / 100) + 48;
    strbuf[8] = ((year % 100) / 10) + 48;
    strbuf[9] = (year % 10) + 48;
    strbuf[10] = 0;
}

void StringUtils::timeToString(uint8_t hours, uint8_t minutes, uint8_t seconds, char *strbuf)
{
    // Convert time from numbers to a string (hh:mm:ss)
    // 48 decimal is ascii 0
    strbuf[0] = (hours / 10) + 48;
    strbuf[1] = (hours % 10) + 48;
    strbuf[2] = ':';
    strbuf[3] = (minutes / 10) + 48;
    strbuf[4] = (minutes % 10) + 48;
    strbuf[5] = ':';
    strbuf[6] = (seconds / 10) + 48;
    strbuf[7] = (seconds % 10) + 48;
    strbuf[8] = 0;
}
