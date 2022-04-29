/***************************************************
 Library for network stuff, connection, AP and so on.

 License: Creative Common V1. 

 Benjamin Völker, voelkerb@me.com
 Embedded Systems Engineer
 ****************************************************/

#ifndef NETWORK_h
#define NETWORK_h

// Ethernet stuff
#if defined(ESP32)
// Both to set bt to sleep
#include <ETH.h>
#include <esp_sleep.h>
#include <esp_bt.h>
// Wifi stuff
#include <esp_wifi.h>
// Wifi and TCP/UDP Server stuff
#include <WiFi.h>
#include <WiFiAP.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <Ticker.h> 
#else 
#error Not supported for this architecture!
#endif

// Logging and configuration stuff
// #include <multiLogger.h>

#if (ARDUINO >= 100)
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
// #define DEBUG_DEEP
#define CHECK_PERIODE_MS 30000

namespace Network {
    extern bool connected;
    extern bool apMode;
    extern bool ethernet;
    extern bool allowNetworkChange;

    #if defined(ESP8266)
    void update();
    #endif

    void init(char * name, char * SSID, char * pwd);
    void init(char * name, char * SSID, char * pwd, void (*onConnect)(void), void (*onDisconnect)(void), bool usingEthernet=false, void (*_logFunc)(const char * msg, ...)=NULL);
    
#ifdef _ETH_H_
    void initPHY(uint8_t addr, uint8_t pwr, uint8_t mdc, uint8_t mdio, eth_phy_type_t type, eth_clock_mode_t clk_mode);
#endif
    char * getBSSID();
    bool connect(char * network, char * pswd);
    void setupAP();
    IPAddress localIP();
    void wifiEvent(WiFiEvent_t event);
    void checkNetwork();

    void scanNetwork();
}

#endif
