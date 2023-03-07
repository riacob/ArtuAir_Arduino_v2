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

// Serial buffer
char sbuf[64] = {0};

void oledPrint(int state);

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
    digitalWrite(PIN_LED_RED, !digitalRead(PIN_LED_RED));
  }

  /* Serial0 data (HDLC) */
  if (Serial.available() > 4)
  {
    for(int i = 0; i < 64; i++){
      sbuf[i] = 0;
    }
    uint8_t framelgt = 1;
    // We have an HDLC frame
    if (Serial.read() == '~')
    {
      // The flag was removed by the first call to Serial.read()
      sbuf[0] = '~';
      // Read frame until flag or until max serial buffer size is reached
      do
      {
        if(Serial.available()){
          sbuf[framelgt] = Serial.read();
          framelgt++;
        }        
      } while ((sbuf[framelgt-1] != '~') && (framelgt < 64));
      Serial.flush();
      oled.clearDisplay();
      oled.setCursor(0,0);
      oled.setTextColor(WHITE);
      oled.setTextSize(3);
      oled.print(sbuf[0]);
      oled.print(sbuf[1]);
      oled.print(sbuf[2]);
      oled.display();
      delay(5000);
    }
  }
}

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