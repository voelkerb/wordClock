/***************************************************
 Library for Configuration handling.

 Feel free to use the code as it is.

 Benjamin Voelker, voelkerb@me.com
 Embedded Systems
 University of Freiburg, Institute of Computer Science
 ****************************************************/

#include "config.h"

Configuration::Configuration() {
  snprintf(myConf.SSID, MAX_STRING_LEN, "\0");
  snprintf(myConf.PWD, MAX_STRING_LEN, "\0");
  snprintf(myConf.mqttServer, MAX_DNS_LEN, NO_SERVER);
  snprintf(myConf.timeServer, MAX_DNS_LEN, "time.google.com");
  snprintf(myConf.mqttUser, MAX_STRING_LEN, "\0");
  snprintf(myConf.mqttPwd, MAX_STRING_LEN, "\0");
  myConf.mqttPort = 1883;
  myConf.colorOrder = RGB;
}

void Configuration::init() {
  // Load the MDNS name from eeprom
  EEPROM.begin(EEPROM_SIZE);
  static_assert(EEPROM_SIZE <= 512, "Max eeprom size is 512");
}

void Configuration::makeDefault(bool resetName) {
  // We can do a reset but keep the name
  if (resetName) {
    snprintf(myConf.name, MAX_STRING_LEN, "clockTwo");
  }
  snprintf(myConf.SSID, MAX_STRING_LEN, "\0");
  snprintf(myConf.PWD, MAX_STRING_LEN, "\0");
  snprintf(myConf.mqttServer, MAX_DNS_LEN, "-");
  snprintf(myConf.timeServer, MAX_DNS_LEN, "time.google.com");
  snprintf(myConf.mqttUser, MAX_STRING_LEN, "\0");
  snprintf(myConf.mqttPwd, MAX_STRING_LEN, "\0");
  myConf.mqttPort = 1883;
  myConf.brightness = 64;
  myConf.fgColor = CRGB(100,100,100);
  myConf.bgColor = CRGB(0,0,0);
  myConf.nightColor = CRGB(5,5,5);
  myConf.secondsNightStart = 0;
  myConf.secondsNightStop = 6*60*60;
  myConf.colorOrder = RGB;

  myConf.valid = 0x1234;
}


void Configuration::load() {
  uint32_t address = NAME_START_ADDRESS;
  EEPROM.get(address, myConf);
  address += sizeof(myConf);
  if (myConf.valid != 0x1234) {
    Serial.printf("\nConfig not valid, making default\n");
    makeDefault();
    store();
  }
}

void Configuration::store() {
  myConf.valid = 0x1234;
  uint32_t address = NAME_START_ADDRESS;
  EEPROM.put(address, myConf);
  address += sizeof(myConf);
  EEPROM.commit();
}


void Configuration::setMQTTServer(char * serverAddress, uint16_t port, char * user, char * pwd) {
  snprintf(myConf.mqttServer, MAX_DNS_LEN, serverAddress);
  snprintf(myConf.mqttUser, MAX_STRING_LEN, user);
  snprintf(myConf.mqttPwd, MAX_STRING_LEN, pwd);
  myConf.mqttPort = port; 
  store();
}


void Configuration::setTimeServerAddress(char * serverAddress) {
  snprintf(myConf.timeServer, MAX_DNS_LEN, serverAddress);
  store();
}

void Configuration::setName(char * newName) {
  snprintf(myConf.name, MAX_STRING_LEN, newName);
  store();
}

void Configuration::setNetwork(char * ssid, char * pwd) { 
  snprintf(myConf.SSID, MAX_NETWORK_LEN, ssid);
  snprintf(myConf.PWD, MAX_PWD_LEN, pwd);
  store();
}
