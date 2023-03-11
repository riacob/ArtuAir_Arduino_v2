/**
 * @file main.cpp
 * @authors 5ELIA Malignani Udine A.S. 2022/2023
 * @brief
 * @version 0.1
 * @date 2023-03-06
 *
 * @copyright Copyright (c) 2023
 *
 */

/* Arduino libraries */
#include <Arduino.h>
#include <Wire.h>

/* Adafruit libraries */
#include <Adafruit_BME680.h>
#include <Adafruit_SSD1306.h>
#include <RTClib.h>

/* Internal libraries and headers */
#include "hardware.h"
#include "stringutils.h"
#include "hdlc.h"

/* Devices */
// Bosch SensorTech BME680 ambient parameter sensor
Adafruit_BME680 bme680(&Wire);
// 128x64 PX 0.96" OLED display
Adafruit_SSD1306 oled(128, 64, &Wire, -1);
// High-precision external RTC module
RTC_DS3231 rtc;

/* Timers */
// Display refresh
unsigned long ms0 = 0;
// Perform measurement
unsigned long ms1 = 0;
// Activity led
unsigned long ms2 = 0;
// Serial wait time
unsigned long msS = 0;

// Serial buffer
uint8_t sbuf[64];

// HDLC object
HDLC hdlc(sbuf, 64);

void oledPrint(int state);
void handleSerialReading(HardwareSerial *serial);
void handleSerialWriting(HardwareSerial *serial, uint8_t *data, int datalen);
void handleReceiveHDLC(HDLC::HDLCData *data);

void setup()
{
  /* Initialize devices and peripherals */
  Serial.begin(BAUDRATE_SERIAL);
  Serial1.begin(BAUDRATE_SERIALBT);
  oled.begin(SSD1306_SWITCHCAPVCC, I2C_SSD1306_ADD);
  bme680.begin(I2C_BME680_ADD, true);
  rtc.begin(&Wire);
  // Print welcome screen
  oledPrint(0);

  pinMode(PIN_LED_BLUE, OUTPUT);
  pinMode(PIN_LED_RED, OUTPUT);
  pinMode(PIN_LED_GREEN, OUTPUT);
  pinMode(PIN_LED_YELLOW, OUTPUT);

  // Get the first data from the sensor
  bme680.performReading();

  delay(3000);
}

void loop()
{
  /* Update display every 100ms */
  if (millis() - ms0 >= 100)
  {
    ms0 = millis();
    oledPrint(1);
  }

  /* Update data sensor every 60s */
  if (millis() - ms1 >= 60000)
  {
    ms1 = millis();
    bme680.performReading();
  }

  /* Blink led with a frequency of 1Hz */
  if (millis() - ms2 >= 1000)
  {
    ms2 = millis();
    // digitalWrite(PIN_LED_RED, !digitalRead(PIN_LED_RED));
  }

  /* Serial0 data (HDLC) */
  // if in the serial buffer there are more than 6 bytes (the minimum number of bytes requested for HDLC)
  // it starts reading
  if (Serial.available() > 6)
  {
    handleSerialReading(&Serial);
  }
}

/**
 * @brief Prints on the oled dispay a pregiven image base on the given state code
 *
 * @param state The number of the state that will be printed
 */
void oledPrint(int state)
{
  switch (state)
  {
  case 0:
  {
    oled.clearDisplay();
    oled.setCursor(0, 0);
    oled.setTextColor(WHITE);
    oled.setTextSize(3);
    oled.println("ArtuAir");
    oled.setTextSize(1);
    oled.println();
    oled.println("   Malignani Udine");
    oled.println(" 5ELIA A.S. 2022/2023");
    oled.display();
    break;
  }
  case 1:
  {
    char strbuf[16];
    DateTime dt = rtc.now();
    oled.clearDisplay();
    oled.setCursor(0, 0);
    oled.setTextColor(WHITE);
    oled.setTextSize(1);
    StringUtils::dateToString(dt.day(), dt.month(), dt.year() - 100, strbuf);
    oled.print(strbuf);
    oled.print(" ");
    StringUtils::timeToString(dt.hour(), dt.minute(), dt.second(), strbuf);
    oled.println(strbuf);
    oled.println();
    oled.print("Temperature: ");
    oled.print(bme680.temperature);
    oled.println(" C");
    oled.print("Humidity: ");
    oled.print(bme680.humidity);
    oled.println(" %");
    oled.print("Pressure: ");
    oled.print(bme680.pressure / 100.0);
    oled.println(" Pa");
    oled.print("Gas: ");
    oled.print(bme680.gas_resistance);
    oled.println(" Ohm");
    oled.display();
    break;
  }

  default:
  {
    break;
  }
  }
}

/**
 * @brief Handle the serial reading and the eventually command execution and response
 *
 * @param serial The serial device to read the data from
 */
void handleSerialReading(HardwareSerial *serial)
{
  // We have an HDLC frame
  if (serial->read() == '~')
  {

    for (int i = 0; i < 64; i++)
    {
      sbuf[i] = 0;
    }

    // Length of the HDLC frame
    uint8_t frameidx = 1;
    // The flag was removed by the first call to Serial.read()
    sbuf[0] = '~';
    // Read frame until flag or until max serial buffer size is reached or until data is no longer available
    do
    {
      if (serial->available())
      {
        msS = millis();
        sbuf[frameidx] = serial->read();
        frameidx++;
      }
    } while ((sbuf[frameidx - 1] != '~') && (frameidx < 64) && (millis() - msS < 100));
    serial->flush();
    // CRC16 is valid
    if (hdlc.unframe())
    {

      // Read incoming frame
      HDLC::HDLCData *hdlcd = hdlc.getData();

      // Transmit frame containing sequence number back to the sender
      HDLC::HDLCData seqn;

      seqn.ADD = 0x00;
      seqn.CTR = 0x00;
      seqn.DAT = &(hdlcd->CTR);
      seqn.DATlen = 1;
      hdlc.setData(&seqn);
      int len = hdlc.frame();
      // handleSerialWriting(serial, sbuf, len);

      // Do something with the data
      handleReceiveHDLC(hdlcd);
    }
    // If the crc is not correct the serial buffer gets flushed, it then continue in the loop waiting
    // for the other device to send the message again
    else
    {
      // serial->println("invalid crc");
      serial->flush();
    }
  }
  else
  {
    serial->flush();
  }
}

/**
 * @brief Writes on the given serial bus the given data array
 *
 * @param serial The serial bus to write on
 * @param data The data to be write (uint8_t) array
 * @param datalen The lenght of the data array
 */
void handleSerialWriting(HardwareSerial *serial, uint8_t *data, int datalen)
{
  for (int i = 0; i < datalen; i++)
  {
    serial->write(data[i]);
  }
}

/**
 * @brief Select and execute the request arrived through the serial line
 *
 * @param data The HDLC data arrived through serial
 */
void handleReceiveHDLC(HDLC::HDLCData *data)
{

  if (!(data->ADD == SERIAL_ADD))
  {
    return;
  }

  // get the command that will be executed
  uint8_t cmd = data->DAT[0];

  switch (cmd)
  {
  case CMD_RECV_DATE:
  {
    oled.clearDisplay();
    oled.setTextColor(WHITE);
    oled.setTextSize(2);
    oled.setCursor(0, 0);
    oled.print(data->DATlen);
    oled.display();
    delay(5000);
    break;
  }
  case CMD_RECV_TIME:
  {
    break;
  }
  case CMD_RECV_ACT_DOW:
  {
    break;
  }
  case CMD_RECV_ACT_TIME:
  {
    break;
  }
  case CMD_RECV_PASSW_TRY:
  {
    break;
  }
  default:
  {
    break;
  }
  }
}