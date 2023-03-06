/**
 * @file hardware.h
 * @authors 5ELIA Malignani Udine A.S. 2022/2023
 * @brief
 * @version 0.1
 * @date 2023-03-06
 *
 * @copyright Copyright (c) 2023
 */

#ifndef HARDWARE_H
#define HARDWARE_H

/* Serial baudrates */
// Serial0
#define BAUDRATE_SERIAL 115200
// Serial1
#define BAUDRATE_SERIALBT 38400

/* I2C device addresses */
// RTC module
#define I2C_DS3231_ADD 0x68
// BME680 sensor
#define I2C_BME680_ADD 0x77
// OLED display
#define I2C_SSD1306_ADD 0x3C
// External EEPROM
#define I2C_EEPROM_ADD 0x57

/* GPIOs */
// Green LED
#define PIN_LED_GREEN 52
// Yellow LED
#define PIN_LED_YELLOW 51
// Red LED
#define PIN_LED_RED 49
// Blue LED
#define PIN_LED_BLUE 47

#endif