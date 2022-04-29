/***************************************************
 Library for rtc time handling.
 
 License: Creative Common V1. 

 Benjamin Völker, voelkerb@me.com
 Embedded Systems Engineer
 ****************************************************/

#ifndef RTC_h
#define RTC_h

#include <Wire.h> // must be included here so that Arduino library object file references work
#include <RTClib.h>

#define _MAX_RTC_TIME_STR_LENGTH 50
#define MAX_UPDATE_SPEED 10
#if (ARDUINO >= 100)
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

class Rtc {
  public:
    Rtc(int8_t intPin, int8_t sda = -1, int8_t scl = -1);
    bool init();
    DateTime update();
    bool enableInterrupt(int frequency, void (*cb)(void));
    void disableInterrupt();
    void setTime(DateTime dt);
    char * timeStr(DateTime dt);
    char * timeStr();
    bool enableRTCSqwv(int frequency);

    bool connected;
    bool lost;
    const int8_t _SDA;
    const int8_t _SCL;

    DateTime _now;

  private:
    int INT_PIN;
    char _timeStr[_MAX_RTC_TIME_STR_LENGTH];

#if defined(ESP32)
    static void _DS3231_Interrupt(void * pvParameters);
    TaskHandle_t _interruptHandle;
#endif

    unsigned long _lastRequest;

    void (*_intCB)(void);
};

#endif
