// This code is based on the JeeLab's RTC libarary for Arduino
// So why add the MCP79410? Well, it has a wider input voltage range, both
// in Vin and Vbatt (backup), as well as all of the standard functionality
// of a chip like the DS1307 with onboard NVRAM
/***************************************************************************
* File Name: mcp79410.ino
* Processor/Platform: Arduino Uno R3 (tested)
* Development Environment: Arduino 1.6.5
*
* Originally designed for use with with Playing With Fusion MCP79410 and  
* micro SD Arduino shield, IFB-11001. This example doesn't use the SD, and
* as such, this should work with any Arduino over I2C.
*
* Copyright © 2016 Playing With Fusion, Inc.
* SOFTWARE LICENSE AGREEMENT: This code is released under the MIT License.
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the "Software"),
* to deal in the Software without restriction, including without limitation
* the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
* DEALINGS IN THE SOFTWARE.
* **************************************************************************
* REVISION HISTORY:
* Author          Date        Comments
* J. Steinlage    2016May31   First revision
*
* Playing With Fusion, Inc. invests time and resources developing open-source
* code. Please support Playing With Fusion and continued open-source
* development by buying products from Playing With Fusion!
* **************************************************************************
* ADDITIONAL NOTES:
* This file contains functions to initialize and run an Arduino Uno R3 in
* order to communicate with the MCP74910 on the IFB-11001 RTC and uSD shield
* - Configure Arduino to broadcast results via UART
*       - call updated RTClib library to configure and read MCP79410 RTC
*       - MCP79410 is included on the IFB-11001 RTC/microSD shield
* - Set current or user-defined time
* - enable battery backup on RTC
* - Read current time from RTC
* - Calculate different times using update RTClib source functions
*  Circuit (for reference only... install full header set and place on Arduino):
*    Arduino Uno   Arduino Mega  -->  Shield / Function
*    D6            D6            -->  IFB-11001    / Card Detect - SD (not used in this example)
*    D5            D5            -->  IFB-11001    / Chip Select, SD  (not used in this example)
*    MOSI: pin 11  MOSI: pin 51  -->  IFB-11001    / SDI via ICSP     (not used in this example)
*    MISO: pin 12  MISO: pin 50  -->  IFB-11001    / SDO via ICSP     (not used in this example)
*    SCK:  pin 13  SCK:  pin 52  -->  IFB-11001    / SCLK via ICSP    (not used in this example)
*    SCL           SCL           -->  IFB-11001    / SCL  - used for RTC
*    SDA           SDA           -->  IFB-11001    / SDA  - used for RTC
*    GND           GND           -->  GND
*    5V            5V            -->  5V (supply with same voltage as Arduino I/O, 5V)
*    VCCIO         VCCIO         -->  Vcc (supply with same voltage as Arduino I/O, 3.3-5V)
*    3.3V          3.3V          --> 3.3V (this is 3.3V output from Arduino LDO)
***************************************************************************/

// Date and time functions using a MCP79410 RTC connected via I2C and Wire lib
#include "Wire.h"
#include "RTClib.h"

#if defined(ARDUINO_ARCH_SAMD)
// for Zero, output on USB Serial console, remove line below if using programming port to program the Zero!
   #define Serial SerialUSB
#endif

RTC_MCP79410 rtc;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

void setup () {

#ifndef ESP8266
  while (!Serial); // for Leonardo/Micro/Zero
#endif

  Serial.begin(115200);
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);  // don't go anywhere!
  }

  // Check to see if RTC is running. If not, set the date to settings below. If it is, assume
  // that it is set correctly, and doesn't need to be adjusted. REMOVE THIS CHECK IF YOU WANT 
  // TO OVERRIDE THE CURRENT RTC SETTINGS!
  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running, setting date/time!");
    // following line sets the RTC to the date & time this sketch was compiled
    //                    Y   M  D   H   M   S    enable battery backup
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)), MCP7941X_BATT_BKUP_EN);
    // To set the RTC with an explicit date & time, see below
    // For May 31, 2016 at 1:23:16 : 
    //                      Y   M  D   H   M   S    enable battery backup
    //rtc.adjust(DateTime(2016, 5, 31, 1, 23, 16), MCP7941X_BATT_BKUP_EN);
  }
  else{Serial.println("RTC already running... no time adjust");}
}

void loop () {
    DateTime now = rtc.now();
    Serial.print(now.day(), DEC);
    Serial.print(" (");
    Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
    Serial.print(") ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();
    
    Serial.print(" since midnight 1/1/1970 = ");
    Serial.print(now.unixtime());
    Serial.print("s = ");
    Serial.print(now.unixtime() / 86400L);
    Serial.println("d");
    
    // calculate a date which is 7 days, 12 hours, 30 minutes and 6 seconds into the future
    DateTime future (now + TimeSpan(7,12,30,6));
    
    Serial.print(" now + 7d, 12h, 30m, 6s: ");
    Serial.print(future.year(), DEC);
    Serial.print('/');
    Serial.print(future.month(), DEC);
    Serial.print('/');
    Serial.print(future.day(), DEC);
    Serial.print(' ');
    Serial.print(future.hour(), DEC);
    Serial.print(':');
    Serial.print(future.minute(), DEC);
    Serial.print(':');
    Serial.print(future.second(), DEC);
    Serial.println();
    
    Serial.println();
    delay(3000);
}
