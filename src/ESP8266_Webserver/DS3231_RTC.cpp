/***************************************************
 Library for rtc time handling.

 License: Creative Common V1. 

 Benjamin Völker, voelkerb@me.com
 Embedded Systems Engineer
 ****************************************************/

#include "DS3231_RTC.h"

#define DEBUG_RTC

RTC_DS3231 _rtc;
#if defined(ESP32)
xQueueHandle DS3231_xQueue = xQueueCreate(100, sizeof(bool));
static void _DS3231_Interrupt_ISR() {
  bool success = false;
  BaseType_t xHigherPriorityTaskWoken;
  xQueueSendToBackFromISR( DS3231_xQueue, &success, &xHigherPriorityTaskWoken );
}
#endif

Rtc::Rtc(int8_t intPin, int8_t sda, int8_t scl) :_SDA(sda), _SCL(scl) {
  INT_PIN = intPin;
  _intCB = NULL;
  lost = true;
  connected = false;
#if defined(ESP32)
  _interruptHandle = NULL;
#endif
  _lastRequest = 0;
  _now = DateTime(0);
}


bool Rtc::init() {

  if (INT_PIN >= 0) pinMode(INT_PIN, INPUT_PULLUP);
  // RTC.begin just does wire.begin but does not have ability to pass different SDA SCL pins
  if (_SDA != -1) {
    Wire.begin(_SDA, _SCL);
    // Wire.begin(_SDA, _SCL, 400000);
    #ifdef DEBUG_RTC
    Serial.printf("Using I2C pins: SDA %i, SCL: %i\n", _SDA, _SCL);
    #endif
  }
  else _rtc.begin();

#if defined(ESP32)
  // Wire.setDebugFlags(0xffff, 0xffff);
#endif
  // Read temperature to see a DS3231 is connected
  float temp = _rtc.getTemperature();
#ifdef DEBUG_RTC
  Serial.printf("Temp: %.2f\n", temp);
#endif
  // If temperature is beyond normal, consider RTC to be not present
  if (temp < 1 || temp > 70) {
  #ifdef DEBUG_RTC
    Serial.printf("No RTC connected\n");
  #endif
    connected = false;
    return false;
  } else {
    connected = true;
  }
  // Indicate power loss, s.t. others know they cannot trust time
  if (_rtc.lostPower()) {
    lost = true;
    // Set rtc time to compile time
    _rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  #ifdef DEBUG_RTC
  Serial.printf("Temp: %.2f\n", _rtc.getTemperature());
  DateTime dt = update();
  sprintf(_timeStr, "%02u/%02u/%04u %02u:%02u:%02u", dt.month(), dt.day(),
            dt.year(),
            dt.hour(),
            dt.minute(),
            dt.second() );
  Serial.printf("Time: %s\n", _timeStr);
  #endif
  return true;
}

void Rtc::setTime(DateTime dt) {
  // Set the new time
  _rtc.adjust(dt);
  // Indicate time can be trusted now
  lost = false;
}


bool Rtc::enableInterrupt(int frequency, void (*cb)(void)) {
  if (INT_PIN == -1) {
#ifdef DEBUG_RTC
    Serial.println("Need to init RTC with Pin to which RTC SQW out is connected");
#endif
    return false;
  }
  bool success = enableRTCSqwv(frequency);
  if (!success) return false;
  _intCB = cb;

#if defined(ESP32)
  if (_interruptHandle == NULL) {
      // let it check on second core (not in loop)
      Rtc *obj = this;
      xTaskCreatePinnedToCore(_DS3231_Interrupt,          /* Task function. */
                  "_DS3231_Interrupt",        /* String with name of task. */
                  10000,            /* Stack size in bytes. */
                  (void*) obj,       /* Task input parameter */
                  3,                /* Priority of the task. */
                  &_interruptHandle,1);            /* Task handle. */
  }
  attachInterrupt(digitalPinToInterrupt(INT_PIN), &_DS3231_Interrupt_ISR, FALLING);
#else
  // Arduino version
  attachInterrupt(digitalPinToInterrupt(INT_PIN), _intCB, FALLING);
#endif
  return true;
}

bool Rtc::enableRTCSqwv(int frequency) {
  // TODO: does this reset the sqwc counter?
  _rtc.adjust(_now);
  if (frequency == 1) {
    _rtc.writeSqwPinMode(DS3231_SquareWave1Hz);
  } else if (frequency == 1024) {
    _rtc.writeSqwPinMode(DS3231_SquareWave1kHz);
  } else if (frequency == 4096) {
    _rtc.writeSqwPinMode(DS3231_SquareWave4kHz);
  } else if (frequency == 8192) {
    _rtc.writeSqwPinMode(DS3231_SquareWave8kHz);
  } else {
#ifdef DEBUG_RTC
    Serial.println("Unsupported RTC SQWV frequency");
#endif
    return false;
  }
  // TODO: Frequency calculation and so on...
  // Currently 1 Hz
  // _rtc.SetSquareWavePin(DS3231SquareWavePin_ModeClock);
  return true;
}

void Rtc::disableInterrupt() {
  detachInterrupt(digitalPinToInterrupt(INT_PIN));
  _intCB = NULL;
}

DateTime Rtc::update() {
  if (!connected) {
    // Try init again
    init();
    if (!connected) return DateTime(F(__DATE__), F(__TIME__));
  }
  // Seems like the RTC requires some idle time before another now request is allowed
  if ((millis()-_lastRequest) < MAX_UPDATE_SPEED) return _now;
  _now = _rtc.now();
  _lastRequest = millis();
  return _now;
}

char * Rtc::timeStr() {
  return timeStr(update());
}

char * Rtc::timeStr(DateTime dt) {
    snprintf(_timeStr, _MAX_RTC_TIME_STR_LENGTH, "%02u/%02u/%04u %02u:%02u:%02u", dt.month(), dt.day(),
            dt.year(),
            dt.hour(),
            dt.minute(),
            dt.second() );
    return &_timeStr[0];
}

#if defined(ESP32)
// _________________________________________________________________________
// Decorator function since we cannot use non static member function in freertos' createTask 
void Rtc::_DS3231_Interrupt(void * pvParameters) {
  bool success = false;
  while(true) {
    BaseType_t xStatus = xQueueReceive( DS3231_xQueue, &success, portMAX_DELAY);
    if(xStatus == pdPASS) {
      if (xPortGetCoreID() != 0 && ((Rtc*)pvParameters)->connected) {
        if (((Rtc*)pvParameters)->_intCB != NULL) ((Rtc*)pvParameters)->_intCB();
      }
    }
  }
  ((Rtc*)pvParameters)->_interruptHandle = NULL;
  // Delete this task
  vTaskDelete(NULL);
}
#endif