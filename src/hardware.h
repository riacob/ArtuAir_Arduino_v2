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

//Arduino Serial Address
#define SERIAL_ADD 0x1E

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
#define PIN_LED_GREEN 53
// Yellow LED
#define PIN_LED_YELLOW 51
// Red LED
#define PIN_LED_RED 49
// Blue LED
#define PIN_LED_BLUE 47

// List of possible commands that the arduino may receive through serial
  enum Commands {
    // Send temperature
    CMD_SEND_TEMPERATURE = 0,
    // Send humidity
    CMD_SEND_HUMIDITY = 1,
    // Send pressure
    CMD_SEND_PRESSURE = 2,
    // Send indoor air quality
    CMD_SEND_IAQ = 3,
    // Send RTC date
    CMD_SEND_DATE = 4,
    // Send RTC time
    CMD_SEND_TIME = 5,
    // Receive RTC date
    CMD_RECV_DATE = 6,
    // Receive RTC time
    CMD_RECV_TIME = 7,
    // Receive active days of week
    CMD_RECV_ACT_DOW = 8,
    // Receive active time slot
    CMD_RECV_ACT_TIME = 9,
    // Receive password attempt
    CMD_RECV_PASSW_TRY = 10
  };

#endif