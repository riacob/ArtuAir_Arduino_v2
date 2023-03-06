/**
 * @file stringutils.h
 * @authors 5ELIA Malignani Udine A.S. 2022/2023
 * @brief
 * @version 0.1
 * @date 2023-03-06
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef STRINGUTILS_H
#define STRINGUTILS_H

#include <Arduino.h>

class StringUtils
{
public:
    /**
     * @brief Convert day, month and year from numbers into a string with the following format: dd/mm/yyyy
     * @note strbuf must be at least 11 elements long
     *
     * @param day: The day
     * @param month: The month
     * @param year: The year
     * @param strbuf: The string buffer the date string will be written to
     */
    static void dateToString(uint8_t day, uint8_t month, uint16_t year, char *strbuf);
    /**
     * @brief Convert hours, minutes and seconds from numbers into a string with the following format: hh:mm:ss
     * @note strbuf must be at least 9 elements long
     *
     * @param hours: The hours
     * @param minutes: The minutes
     * @param seconds: The seconds
     * @param strbuf: The string buffer the time string will be written to
     */
    static void timeToString(uint8_t hours, uint8_t minutes, uint8_t seconds, char *strbuf);
};

#endif