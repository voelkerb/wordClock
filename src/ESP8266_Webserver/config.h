/***************************************************
 Library for rtc time handling.

 Feel free to use the code as it is.

 Benjamin Völker, voelkerb@me.com
 Embedded Systems
 University of Freiburg, Institute of Informatik
 ****************************************************/

#ifndef CONFIG_h
#define CONFIG_h

#include <EEPROM.h>
#include <FastLED.h>

#if (ARDUINO >= 100)
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

// Include for network config
#include "timeHandling.h"

// If you cahange any of these values, the config on all devices will be bricked
#define MAX_STRING_LEN 25
#define MAX_IP_LEN 17
#define MAX_DNS_LEN 25
#define MAX_NAME_LEN MAX_STRING_LEN

#define NAME_START_ADDRESS 0

#define NO_SERVER "-"

#define EEPROM_SIZE (sizeof(Config_t)+2)

#define MAX_NETWORK_LEN 20
#define MAX_PWD_LEN 30

// packed required to store in EEEPROM efficiently
struct __attribute__((__packed__)) Config_t {
  uint16_t valid = 0x1233;
  char name[MAX_STRING_LEN] = {'\0'};        // Name
  char SSID[MAX_NETWORK_LEN] = {'\0'};       // SSID
  char PWD[MAX_PWD_LEN] = {'\0'};            // PWD
  char mqttServer[MAX_DNS_LEN] = {'\0'};     // MQTT Server 
  char timeServer[MAX_DNS_LEN] = {'\0'};     // Time Server
  char mqttUser[MAX_STRING_LEN] = {'\0'};    // MQTT User 
  char mqttPwd[MAX_STRING_LEN] = {'\0'};     // MQTT pwd
  uint16_t mqttPort = 0;
  uint8_t brightness = 0;
  CRGB fgColor;
  CRGB bgColor;
  CRGB nightColor;
  uint32_t secondsNightStart = 0;
  uint32_t secondsNightStop = 6*60*60;
  uint8_t colorOrder = RGB;
}; 

class Configuration {
  public:
    Configuration();
    void init();
    void load();
    void store();
    // bool loadStoreTo(uint32_t address, bool store, uint8_t * data, size_t size);
    void makeDefault(bool resetName=true);

    void setName(char * newName);
    void setMQTTServer(char * serverAddress, uint16_t port, char * user, char * pwd);
    void setTimeServerAddress(char * serverAddress);
    void setNetwork(char * ssid, char * pwd);

    Config_t myConf;

  private:
    void storeMyConf();
};

#endif
