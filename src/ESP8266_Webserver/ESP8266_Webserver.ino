#define FASTLED_ALLOW_INTERRUPTS 0
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <EEPROM.h>
#include <ESP8266WebServer.h>
#define COLOR_ORDER GRB

#include "EspHtmlTemplateProcessor.h"

#include "FastLED.h"
// #include "website.h"
#include "config.h"
#include "network.h"
#include "DS3231_RTC.h"

#include "timeHandling.h"
#include "clocktwo_defines.h"

void mylog(const char* _log, ...);
void ntpUpdated();

#define DEBUG 0
// #define TIME_DEPENDENT_BRIGHTNESS

#define BRIGHTNESS_ADJUST_RANGE 0.4
#define MIN_START 6*60 +  0
#define MAX_START 9*60 +  0
#define MAX_STOP 18*60 +  0
#define MIN_STOP 20*60 + 30

#define DATA_PIN D3

bool updating = false;
// DS3231_RTC rtc(-1, , int8_t scl) );

Rtc rtc(-1);


bool wifiConnected = false;

Configuration conf;

#define TCP_UPDATE_INTV 200
long tcpTimer = -TCP_UPDATE_INTV;
#define MAX_CLIENTS 3
WiFiServer tcpServer(54321);
// TCP clients and current connected state, each client is assigned a logger
WiFiClient client[MAX_CLIENTS];
bool clientConnected[MAX_CLIENTS] = {false};

// Set web server for configuration on port 80
ESP8266WebServer webServer(80);
EspHtmlTemplateProcessor templateProcessor(&webServer);

// Variable to store the HTTP request


#define MQTT_NEWTRY 30000
long mqttTimer = -MQTT_NEWTRY;
#define MQTT_TOPIC_SEPARATOR '/'
WiFiClient pubClient;
PubSubClient mqttClient(pubClient);
bool mqttConnected = false;

#define COMMAND_MAX_SIZE 300
// Command stuff send over what ever
char command[COMMAND_MAX_SIZE] = {'\0'};
char command2[COMMAND_MAX_SIZE] = {'\0'};
StaticJsonDocument<2*COMMAND_MAX_SIZE> docRcv;
StaticJsonDocument<2*COMMAND_MAX_SIZE> docSend;

#define MAX_TOPIC_LEN 50
char pubSubTopic[MAX_TOPIC_LEN] = {'\0'};
char pubOnTopic[MAX_TOPIC_LEN] = {'\0'};
char pubColorTopic[MAX_TOPIC_LEN] = {'\0'};

DateTime lastM;
DateTime lastS;
DateTime now;
Timestamp lastTs;
Timestamp nowTs;

TimeHandler myTime(conf.myConf.timeServer, 1 * 3600, &rtc, &mylog, &ntpUpdated);

int theHour = 0;
int theMinute = 0;
int theSecond = 0;
uint8_t nightHourStart = NIGHT_HOUR_START;
uint8_t nightHourStop = NIGHT_HOUR_STOP;
uint8_t nightMinuteStart = NIGHT_MINUTE_START;
uint8_t nightMinuteStop = NIGHT_MINUTE_STOP;
uint8_t red = 0;
uint8_t blue = 0;
uint8_t green = 0;
uint8_t backRed = 0;
uint8_t backBlue = 0;
uint8_t backGreen = 0;
uint8_t dayRed = COLOR_RED;
uint8_t dayBlue = COLOR_BLUE;
uint8_t dayGreen = COLOR_GREEN;
uint8_t nightRed = COLOR_RED;
uint8_t nightBlue = COLOR_BLUE;
uint8_t nightGreen = COLOR_GREEN;

bool newTime = true;

CRGB leds[NUM_LEDS_ALL];


void printConfig() {
  mylog("Name: %s", conf.myConf.name);
  mylog("mqttServer: %s", conf.myConf.mqttServer);
  mylog("mqttUser: %s", conf.myConf.mqttUser);
  mylog("mqttPwd: %s", conf.myConf.mqttPwd);
  mylog("mqttPort: %u", conf.myConf.mqttPort);
  mylog("timeServer: %s", conf.myConf.timeServer);
  
  mylog("Brightness: %u", conf.myConf.brightness);
  mylog("Day: %u-%u",conf.myConf.secondsNightStart,conf.myConf.secondsNightStop);
  mylog("DayColor: %u,%u,%u",conf.myConf.fgColor.red,conf.myConf.fgColor.green,conf.myConf.fgColor.blue);
  mylog("BackColor: %u,%u,%u",conf.myConf.bgColor.red, conf.myConf.bgColor.green, conf.myConf.bgColor.blue);
  mylog("NightColor: %u,%u,%u",conf.myConf.nightColor.red, conf.myConf.nightColor.green, conf.myConf.nightColor.blue);
}

void setup() {
  // Start the Serial Monitor
  Serial.begin(115200);
  delay(500);
  SPIFFS.begin();
  conf.init();
  //TODO: reset settings - wipe credentials for testing
  // conf.makeDefault();
  // conf.store();
  
  bool success = rtc.init();

  mylog("\n\nRTC: %s", rtc.connected?"connected":"failed");
  rtc.update();
  mylog("RTC Time: %s", rtc.timeStr());

  conf.load();
  printConfig();
  
  // Init network connection
  Network::init(conf.myConf.name, conf.myConf.SSID, conf.myConf.PWD, onWifiConnect, onWifiDisconnect, false, &mylog);

  if (conf.myConf.colorOrder != RGB && conf.myConf.colorOrder != GRB && conf.myConf.colorOrder != BRG) conf.myConf.colorOrder = RGB; 
  initLEDs();
  FastLED.setBrightness(conf.myConf.brightness);
  for (int i = 0; i < NUM_LEDS_ALL; i++) {
    leds[i] = CRGB(0,0,0);
  }
  FastLED.show();
  // Test all colors
  for (int i = 0; i < NUM_LEDS_ALL; i++) {
    leds[i] = CRGB(50,0,0);
    delayMicroseconds(200);
    FastLED.show();
  }
  delay(100);
  for (int i = 0; i < NUM_LEDS_ALL; i++) {
    leds[i] = CRGB(0,50,0);
    delayMicroseconds(200);
    FastLED.show();
  }
  delay(100);
  for (int i = 0; i < NUM_LEDS_ALL; i++) {
    leds[i] = CRGB(0,0,50);
    delayMicroseconds(200);
    FastLED.show();
  }
  delay(100);
  for (int i = 0; i < NUM_LEDS_ALL; i++) {
    leds[i] = CRGB(50,50,50);
    delayMicroseconds(200);
    FastLED.show();
  }
  delay(100);
  

  webServer.on("/", handleRoot);      //Which routine to handle at root location
  webServer.on("/wifi", handleWifi); //form action is handled here
  webServer.on("/color", handleColor); //form action is handled here
  webServer.on("/general", handleGeneral); //form action is handled here
  webServer.begin();

  setupOTA();
  mqttClient.setCallback(mqttCallback);
  mqttClient.setServer(conf.myConf.mqttServer, conf.myConf.mqttPort);
  setTheTime();

  mylog("Setup done");
}

int fakeSecond = 0;

void loop() {
  Network::update();
  // Arduino OTA
  ArduinoOTA.handle();
  // Do not do anything on ota update
  if (updating) return;

  // Wifi mqtt and tcp are sadly all polling checks
  // handleWifiConnect();
  handleMQTTConnect();
  handleTCPClients();
  webServer.handleClient();          //Handle client requests

  // Get now
  nowTs = myTime.timestamp();
  // Second task
  if (nowTs.seconds == lastTs.seconds) return;
  mylog("");
  lastTs = nowTs;
  // Fake a day fast
  // now = DateTime(myTime.utc_seconds()+fakeSecond);
  // fakeSecond += 1;

  if (nowTs.seconds%60 == 0) {
    setTheTime();
    mylog("Time set");
    wifi_set_sleep_type(NONE_SLEEP_T);
    // lastM = now;
  }
}


String color2String(CRGB color) {
  char str[7] = {'\0'};
  snprintf(str, 7, "%.2X%.2X%.2X", color.red, color.green, color.blue);
  return String(str);
}

String indexProcessor(const String& key) {
  mylog("KEY IS %s", key.c_str());
  if (key == "NAME") return String(conf.myConf.name);
  else if (key == "TIMES") return String(conf.myConf.timeServer);
  else if (key == "MQTT") return String(conf.myConf.mqttServer);
  else if (key == "SSID") return String(conf.myConf.SSID);
  else if (key == "BRIGHTNESS") return String((uint8_t)((float)(conf.myConf.brightness)/255.0*100.0));
  else if (key == "FGCOLOR") return color2String(conf.myConf.fgColor);
  else if (key == "BGCOLOR") return color2String(conf.myConf.bgColor);
  else if (key == "NIGHTCOLOR") return color2String(conf.myConf.nightColor);
  else if (key == "NIGHTSTART") {
    String str = "";
    int h = conf.myConf.secondsNightStart/60/60;
    int m = (conf.myConf.secondsNightStart/60%60);
    if (h < 9) str += "0";
    str += (uint8_t)(h);
    str += ":";
    if (m < 9) str += "0";
    str += (uint8_t)(conf.myConf.secondsNightStart/60%60);
    return String(str);
  } else if (key == "NIGHTSTOP") {
    String str = "";
    int h = conf.myConf.secondsNightStop/60/60;
    int m = (conf.myConf.secondsNightStop/60%60);
    if (h < 9) str += "0";
    str += (uint8_t)(h);
    str += ":";
    if (m < 9) str += "0";
    str += (uint8_t)(conf.myConf.secondsNightStop/60%60);
    return String(str);
  }

  return "oops";
}



//===============================================================
// This routine is executed when you open its IP in browser
//===============================================================
void handleRoot() {
  templateProcessor.processAndSend("/index.html", indexProcessor);
}


//===============================================================
// This routine is executed when you press submit
//===============================================================
void handleGeneral() {
  String MQTT = webServer.arg("MQTT"); 
  String Name = webServer.arg("Name"); 
  String Time = webServer.arg("Time"); 
  String TimeS = webServer.arg("TimeS"); 
  String NightStartS = webServer.arg("NightStart"); 
  String NightStopS = webServer.arg("NightStop"); 

  // Night start night stop
  if (strlen(NightStartS.c_str()) > 4) {
    mylog("NightStart: %s", NightStartS.c_str());
    uint8_t hour = atoi(NightStartS.c_str());
    uint8_t minute = atoi(&NightStartS.c_str()[3]);
    if (hour < 24 && minute < 60) {
      conf.myConf.secondsNightStart = hour*60*60 + minute*60;
    }
  }
  if (strlen(NightStopS.c_str()) > 4) {
    mylog("NightStop: %s", NightStopS.c_str());
    uint8_t hour = atoi(NightStopS.c_str());
    uint8_t minute = atoi(&NightStopS.c_str()[3]);
    if (hour < 24 && minute < 60) {
      conf.myConf.secondsNightStop = hour*60*60 + minute*60;
    }
  }
  // Mqtt server
  if (strlen(MQTT.c_str()) > 2) {
    mylog("MQTT: %s", MQTT.c_str());
    conf.setMQTTServer((char*)MQTT.c_str(), 1883, "", "");
    // Need to reconnect mqtt
    mqttConnected = false;
    mqttClient.disconnect();
  }
  // Timeserver
  if (strlen(TimeS.c_str()) > 2) {
    mylog("TimeS: %s", TimeS.c_str());
    conf.setTimeServerAddress((char*)TimeS.c_str());
  }
  // Name server
  if (strlen(Name.c_str()) > 1) {
    mylog("Name: %s", Name.c_str());
    conf.setName((char*)Name.c_str());
    // Need to reconnect mqtt
    mqttConnected = false;
    mqttClient.disconnect();
  }
  if (strlen(Time.c_str()) > 16) {
    mylog("Time: %s", Time.c_str());
    uint16_t year = atoi(&Time.c_str()[0]);
    uint16_t month = atoi(&Time.c_str()[5]);
    uint16_t day = atoi(&Time.c_str()[8]);
    uint16_t hour = atoi(&Time.c_str()[11]);
    uint16_t minute = atoi(&Time.c_str()[14]);
    uint16_t second = atoi(&Time.c_str()[17]);
    mylog("New Time: %u %u %u %u %u %u", year, month, day, hour, minute, second);
    DateTime dt = DateTime(year, month, day, hour, minute, second);
    myTime.setTime(dt);
    mylog("New Time: %s", myTime.timeStr());
  }
  setTheTime();
  conf.store();

  String s = "<a href='/'> Back </a>";
  webServer.send(200, "text/html", s); //Send web page
}


//===============================================================
// This routine is executed when you press submit
//===============================================================
void handleWifi() {
  String SSID = webServer.arg("SSID"); 
  String PWD = webServer.arg("PWD"); 

  mylog("SSID: %s, PWD: %s", SSID.c_str(), PWD.c_str());
  
  conf.setNetwork((char*)SSID.c_str(), (char*)PWD.c_str());
  Network::checkNetwork();

  String s = "<a href='/'> Back </a>";
  webServer.send(200, "text/html", s); //Send web page
}

CRGB hexToCRGB(const char * hex) {
  const char * pos = hex;
  if (pos[0] == '#') pos++;
  if (strlen(hex) < 3) return CRGB(0,0,0);
  uint8_t rgb[3] = {0};
  for (size_t count = 0; count < 3; count++) {
    // Parse hex string of single byte
    sscanf(pos, "%2hhx", &rgb[count]);
    pos += 2;
  }
  return CRGB(rgb[0],rgb[1],rgb[2]);
}

void initLEDs() {
  if (conf.myConf.colorOrder == GRB) {
    FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS_ALL);
  } else if (conf.myConf.colorOrder == BRG) {
    FastLED.addLeds<WS2812B, DATA_PIN, BRG>(leds, NUM_LEDS_ALL);
  } else {
    FastLED.addLeds<WS2812B, DATA_PIN, RGB>(leds, NUM_LEDS_ALL);
  }
}
//===============================================================
// This routine is executed when you press submit
//===============================================================
void handleColor() {
  String fgColor_S = webServer.arg("fgColor"); 
  String bgColor_S = webServer.arg("bgColor"); 
  String nightColor_S = webServer.arg("nightColor"); 
  String brightness_S = webServer.arg("brightness"); 
  String colorOrder_S = webServer.arg("order");
   
 
  CRGB fgColor = hexToCRGB(fgColor_S.c_str());
  CRGB bgColor = hexToCRGB(bgColor_S.c_str());
  CRGB nightColor = hexToCRGB(nightColor_S.c_str());
  uint8_t brightness = (uint8_t)((float)atoi(brightness_S.c_str()) / 100.0 * 255.0);

  mylog("fgColor: %u %u %u", fgColor.red, fgColor.green, fgColor.blue);
  mylog("bgColor: %u %u %u", bgColor.red, bgColor.green, bgColor.blue);
  mylog("nightColor: %u %u %u", nightColor.red, nightColor.green, nightColor.blue);
  mylog("brightness: %u", brightness);

  conf.myConf.fgColor = CRGB(fgColor);
  conf.myConf.bgColor = CRGB(bgColor);
  conf.myConf.nightColor = CRGB(nightColor);
  conf.myConf.brightness = brightness;
  FastLED.setBrightness(conf.myConf.brightness);
  
  if (strlen(colorOrder_S.c_str()) > 2) {
    if (strcmp(colorOrder_S.c_str(), "RGB") == 0) {
      conf.myConf.colorOrder = RGB;
      mylog("Color order: RGB");
    } else if (strcmp(colorOrder_S.c_str(), "GRB") == 0) {
      conf.myConf.colorOrder = GRB;
      mylog("Color order: GRB");
    } else if (strcmp(colorOrder_S.c_str(), "BRG") == 0) {
      conf.myConf.colorOrder = BRG;
      mylog("Color order: BRG");
    }
    conf.store();
    // Sorry but this needs a restart
    ESP.restart();
    initLEDs();
  }
  
  conf.store();
  setTheTime();
  
  String s = "<a href='/'> Back </a>";
  webServer.send(200, "text/html", s); //Send web page
}






float mapfloat(float x, float in_min, float in_max, float out_min, float out_max) {
 return (float)(x - in_min) * (out_max - out_min) / (float)(in_max - in_min) + out_min;
}

void setTheTime() {
  now = myTime.datetime();
  theHour = now.hour();
  theMinute = now.minute();
  theSecond = now.second();
  backRed =  conf.myConf.bgColor.red;
  backGreen =  conf.myConf.bgColor.green;
  backBlue =  conf.myConf.bgColor.blue;

  // Set Color according to day or night time
  bool night = checkNightMode();
  if (night) {
    red = conf.myConf.nightColor.red;
    green = conf.myConf.nightColor.green;
    blue = conf.myConf.nightColor.blue;
    for (int i = 0; i < NUM_LEDS_ALL ;i++) {
      leds[i] = CRGB(0,0,0);
    }
  } else {
    red = conf.myConf.fgColor.red;
    green = conf.myConf.fgColor.green;
    blue = conf.myConf.fgColor.blue;

    for (int i = 0; i < NUM_LEDS_ALL ;i++) {
      leds[i] = CRGB(conf.myConf.bgColor);
    }
  }

  mylog("%u:%u Uhr, Night: %s", theHour, theMinute, night ? "yes":"no");
     
  boolean nextHour = false;
  
  // ES IST
  int i = ES;
  int j = -1;
  int k = -1;
  setLEDs(timeArray[i][ROW], timeArray[i][COL], timeArray[i][LENGTH]);
  i = IST;
  setLEDs(timeArray[i][ROW], timeArray[i][COL], timeArray[i][LENGTH]);
  
  // Get segment 
  int interval = theMinute / 5;
  int dot = theMinute % 5;
  
  setDot(dot);
  
  #ifdef DEBUG_TIME
    mylog("%u:%u:%u Uhr", theHour, theMinute, theSecond);
  #endif
  
  #ifdef DEBUG_INTERVAL
    mylog("min: %u \tintv: %u \t dot: %u", theMinute, interval, dot);
  #endif
  
  switch (interval) {
    case 0:
      i = -1;
      j = UHR;
      if (DEBUG) logappend("Voll");
      break;
    case 1:
      i = FUENF_MINUTE;
      j = NACH;
      if (DEBUG) logappend("5 nach ");
      break;
    case 2:
      i = ZEHN_MINUTE;
      j = NACH;
      if (DEBUG) logappend("10 nach ");
      break;
    case 3:
      i = VIERTEL_MINUTE;
      j = NACH;
      if (DEBUG)logappend("15 nach ");
      break;
    case 4:
      i = ZWANZIG_MINUTE;
      j = NACH;
      if (DEBUG) logappend("20 nach ");
      break;
    case 5:
      i = FUENF_MINUTE;
      j = VOR;
      k = HALB;
      nextHour = true;
      if (DEBUG) logappend("5 vor halb ");
      break;
    case 6:
      i = HALB;
      nextHour = true;
      if (DEBUG) logappend("halb ");
      break;
    case 7:
      i = FUENF_MINUTE;
      j = NACH;
      k = HALB;
      nextHour = true;
      if (DEBUG) logappend("5 nach halb ");
      break;
    case 8:
      i = ZWANZIG_MINUTE;
      j = VOR;
      nextHour = true;
      if (DEBUG) logappend("20 vor ");
      break;
    case 9:
      i = DREIVIER_MINUTE;
      nextHour = true;
      if (DEBUG) logappend("dreiviertel ");
      break;
    case 10:
      i = ZEHN_MINUTE;
      j = VOR;
      nextHour = true;
      if (DEBUG) logappend("10 vor ");
      break;
    case 11:
      i = FUENF_MINUTE;
      j = VOR;
      nextHour = true;
      if (DEBUG) logappend("5 vor ");
      break;
  }
  if (DEBUG) logflush();
 
  if (i != -1) setLEDs(timeArray[i][ROW], timeArray[i][COL], timeArray[i][LENGTH]);
  if (j != -1) setLEDs(timeArray[j][ROW], timeArray[j][COL], timeArray[j][LENGTH]);
  if (k != -1) setLEDs(timeArray[k][ROW], timeArray[k][COL], timeArray[k][LENGTH]);
  
  int theHour2 = theHour % 12;
  if (nextHour) {
    theHour2++;
    if (theHour2 > 11) {
      theHour2 = 0;
    }
  }
  // HOUR
  if (theHour2 == 1 && interval == 0) i = EIN_HOUR;
  else if (theHour2 == 1 && interval > 0) i = EINS_HOUR;
  else if (theHour2 == 2) i = ZWEI_HOUR;
  else if (theHour2 == 3) i = DREI_HOUR;
  else if (theHour2 == 4) i = VIER_HOUR;
  else if (theHour2 == 5) i = FUENF_HOUR;
  else if (theHour2 == 6) i = SECHS_HOUR;
  else if (theHour2 == 7) i = SIEBEN_HOUR;
  else if (theHour2 == 8) i = ACHT_HOUR;
  else if (theHour2 == 9) i = NEUN_HOUR;
  else if (theHour2 == 10) i = ZEHN_HOUR;
  else if (theHour2 == 11) i = ELF_HOUR;
  else if (theHour2 == 0) i = ZWOELF_HOUR;
  
  setLEDs(timeArray[i][ROW], timeArray[i][COL], timeArray[i][LENGTH]);
  FastLED.show();
  delay(100);
}

void setDot(int dot) {
  switch (dot) {
    case 4:
      leds[DOT_FOUR] = CRGB(red, green, blue);
    case 3:
      leds[DOT_THREE] = CRGB(red, green, blue);
    case 2:
      leds[DOT_TWO] = CRGB(red, green, blue);
    case 1:
      leds[DOT_ONE] = CRGB(red, green, blue);
      break;
  }
}

void setLEDs(int row, int col, int length) {
  int i = 0;
  if (row %2 == 0) {
    i = NUM_LEDS-1 - row*NUM_LEDS_ROW - col;
    for (int j = length-1; j >= 0; j--) {
      leds[i-j] = CRGB(red, green, blue);
    }
  }
  else {
    i = NUM_LEDS-1 - row*NUM_LEDS_ROW - (NUM_LEDS_COL - col); 
    for (int j = length-1; j >= 0; j--) {
      leds[i+j] = CRGB(red, green, blue);
    }
  }
}

void ntpUpdated() {
  setTheTime();
}

bool checkNightMode() {
  uint32_t current = theHour*60*60+theMinute*60;
  bool night = false;
  if (conf.myConf.secondsNightStart < conf.myConf.secondsNightStop) {
    if (conf.myConf.secondsNightStart <= current && current < conf.myConf.secondsNightStop) night = true;
  } else {
    if (current >= conf.myConf.secondsNightStart || current < conf.myConf.secondsNightStop) night = true;
  }
  return night;
}


/****************************************************
 * Decode JSON command from string to json dictionary
 ****************************************************/
bool parseCommand(char * cmd) {
  // Deserialize the JSON document
  DeserializationError error = deserializeJson(docRcv, cmd);
  
  // Test if parsing succeeds.
  if (error) {
    // Remove all unallowed json characters to prevent error 
    uint32_t len = strlen(cmd);
    if (len > 30) len = 30;
    for (size_t i = 0; i < len; i++) {
      if (cmd[i] == '\r' || cmd[i] == '\n' || cmd[i] == '"' || cmd[i] == '}' || cmd[i] == '{') cmd[i] = '_';
    }
    mylog("deserializeJson() failed: %.30s", &cmd[0]);
    return false;
  }
  return true;
}

/****************************************************
 * Indicate that someone else is currently sampling 
 * and we cannot perform the requested action
 * This function just build the string msg for it
 ****************************************************/
void handleJSON() {
  // All commands look like the following:
  // {"cmd":"commandName", "payload":{<possible data>}}
  // e.g. mdns

  JsonObject root = docRcv.as<JsonObject>();

  bool validCmd = false;
  JsonVariant colorVariant = root["color"];
  if (!colorVariant.isNull()) {
    JsonArray color = root["color"].as<JsonArray>();
    dayRed = (uint8_t)color[0].as<int>();
    dayGreen = (uint8_t)color[1].as<int>();
    dayBlue = (uint8_t)color[2].as<int>();
    conf.myConf.fgColor = CRGB(dayRed, dayGreen, dayBlue);
    mylog("Set color to: %u,%u,%u", dayRed, dayGreen, dayBlue);
    validCmd = true;
  }
  JsonVariant bgColorVariant = root["bgColor"];
  if (!bgColorVariant.isNull()) {
    JsonArray mycolor = root["bgColor"].as<JsonArray>();
    backRed = (uint8_t)mycolor[0].as<int>();
    backGreen = (uint8_t)mycolor[1].as<int>();
    backBlue = (uint8_t)mycolor[2].as<int>();
    conf.myConf.bgColor = CRGB(backRed, backGreen, backBlue);
    mylog("Set bg color to: %u,%u,%u", backRed, backGreen, backBlue);
    validCmd = true;
  }
  JsonVariant nightColorVariant = root["nightColor"];
  if (!nightColorVariant.isNull()) {
    JsonArray color = root["nightColor"].as<JsonArray>();
    nightRed = (uint8_t)color[0].as<int>();
    nightGreen = (uint8_t)color[1].as<int>();
    nightBlue = (uint8_t)color[2].as<int>();
    conf.myConf.nightColor = CRGB(nightRed, nightGreen, nightBlue);
    mylog("Set night color to: %u,%u,%u", nightRed, nightGreen, nightBlue);
    validCmd = true;
  }
  JsonVariant nightStartVariant = root["nightStart"];
  if (!nightStartVariant.isNull()) {
    JsonArray time = root["nightStart"].as<JsonArray>();
    nightHourStart = (uint8_t)time[0].as<int>();
    nightMinuteStart = (uint8_t)time[1].as<int>();
    conf.myConf.secondsNightStart = nightHourStart*60*60+nightMinuteStart*60;
    mylog("Set Night start: %u:%u", nightHourStart, nightMinuteStart);
    validCmd = true;
  }
  JsonVariant nightStopVariant = root["nightStop"];
  if (!nightStopVariant.isNull()) {
    JsonArray time = root["nightStop"].as<JsonArray>();
    nightHourStop = (uint8_t)time[0].as<int>();
    nightMinuteStop = (uint8_t)time[1].as<int>();
    conf.myConf.secondsNightStop = nightHourStop*60*60+nightMinuteStop*60;
    mylog("Set Night stop: %u:%u", nightHourStop, nightMinuteStop);
    validCmd = true;
  }

  if (validCmd) {
    conf.store();
    // storeEEPROM();
    printConfig();
    setTheTime();
  } else {
    mylog("unknown cmd");
  }
  char colors[12] = {'\0'};
  snprintf(colors, 12, "%u,%u,%u", dayRed, dayGreen, dayBlue);
  if (mqttClient.connected()) {
    mqttClient.publish(pubColorTopic, colors);
  }
}

void mqttCallback(char* topic, byte* message, unsigned int length) {
  memcpy(&command[0], message, length);
  command[length] = '\0';
  mylog("MQTT msg on topic: %s: %s", topic, command);

  // Search for last topic separator
  size_t topicLen = strlen(topic);
  // On not found, this will start from the beginning of the topic string
  int lastSep = -1;
  for (size_t i = 0; i < topicLen; i++) {
    if (topic[i] == '\0') break;
    if (topic[i] == MQTT_TOPIC_SEPARATOR) lastSep = i;
  }
  char * topicEnd = &topic[lastSep+1];

  mylog("Last Seperator: %s",topicEnd);

  char *p = topic;
  char *start;
  while ((start = strtok_r(p, "/", &p)) != NULL) {
    sprintf(&command2[0], "%s", start);
    mylog(command2);
  }

  if (strcmp(topicEnd, "cmd") == 0) {
    JsonObject obj = docSend.to<JsonObject>();
    obj.clear();

    bool success = parseCommand(&command[0]);
    if (not success) {
      mylog("Error Parsing command");
    } else {
      handleJSON();
    }
  } else if (strcmp(topicEnd, "on") == 0) {
    if (mqttClient.connected()) {
      mqttClient.publish(pubOnTopic, "1");
    }
  } else if (strcmp(topicEnd, "color") == 0) {
    char colors[12] = {'\0'};
    snprintf(colors, 12, "%u,%u,%u", dayRed, dayGreen, dayBlue);
    if (mqttClient.connected()) {
      mqttClient.publish(pubColorTopic, colors);
    }
  }
}

void subscribeMQTT() {
  sprintf(pubSubTopic, "%s/set/+", conf.myConf.name);
  sprintf(pubOnTopic, "%s/get/on", conf.myConf.name);
  sprintf(pubColorTopic, "%s/get/color", conf.myConf.name);
  mqttClient.subscribe(pubSubTopic);
  mqttClient.loop();
  mylog("MQTT subscribed to: %s", pubSubTopic);
}

void onMqttDisconnect() {
  mylog("MQTT Disconnected");
}

void onMqttConnect() {
  mylog("MQTT server %s connected", conf.myConf.mqttServer);
  subscribeMQTT();
}

void onWifiDisconnect() {
  mylog("Wifi Disconnected");
  myTime.networkConnected = false;
  wifiConnected = false;
}

void onWifiConnect() {
  if (!Network::apMode) {
    mylog("Connection to Wifi successfull");
    mylog("IP: %s", WiFi.localIP().toString().c_str());
    wifiConnected = true;
    myTime.networkConnected = true;
    myTime.updateNTPTime();

    initMDNS();
  } else {
    mylog("Soft AP opened");
    wifiConnected = true;
    initMDNS();
  }
  tcpServer.stop();
  tcpServer.begin();
}

/****************************************************
 * Init the MDNs name from eeprom, only the number ist
 * stored in the eeprom, construct using prefix.
 ****************************************************/
void initMDNS() {
  if (!MDNS.begin(conf.myConf.name)) { 
    mylog("Error Setting up MDNS responder!");
  }
  MDNS.addService("_clocktwo", "_tcp", 54321);
}

void onClientConnect(WiFiClient &newClient) {
  mylog("Client with IP %s connected on port %u", newClient.remoteIP().toString().c_str(), newClient.remotePort());
}

void onClientDisconnect(WiFiClient &oldClient) {
  mylog("Client discconnected %s port %u", oldClient.remoteIP().toString().c_str(), oldClient.remotePort());
}


/****************************************************
 * Setup the OTA updates progress
 ****************************************************/
// To display only full percent updates
unsigned int oldPercent = 0;
void setupOTA() {
  // Same name as mdns
  ArduinoOTA.setHostname(conf.myConf.name);
  ArduinoOTA.setPassword("clocktwo"); 

  ArduinoOTA.onStart([]() {
    mylog("Start updating");
    updating = true;
    // Disconnecting all connected clients
    for (size_t i = 0; i < MAX_CLIENTS; i++) {
      if (clientConnected[i]) {
        onClientDisconnect(client[i]);
        clientConnected[i] = false;
      }
    }
    tcpServer.stop(); 
    tcpServer.close(); 
  });

  ArduinoOTA.onEnd([]() {
    Serial.println("End");
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    unsigned int percent = (progress / (total / 100));
    if (percent != oldPercent) {
      Serial.printf("Progress: %u%%\r\n", (progress / (total / 100)));
      oldPercent = percent;
    }
  });
  
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
    // No matter what happended, simply restart
    ESP.restart();
  });
  // Enable OTA
  ArduinoOTA.begin();
}

void handleTCPClients() {
  if ((long)(millis() - tcpTimer) < 0) return;
  tcpTimer += TCP_UPDATE_INTV;
  if ((long)(millis() - tcpTimer) < 0) tcpTimer = millis() + TCP_UPDATE_INTV; 
    
  // Handle client connect
  WiFiClient newClient = tcpServer.available();
  if (newClient) {
    bool added = false;
    // Loop over all clients and look where we can store the pointer... 
    for (size_t i = 0; i < MAX_CLIENTS; i++) {
      if (!clientConnected[i]) {
        client[i] = newClient;
        // Set connected flag
        clientConnected[i] = true;
        added = true;
        onClientConnect(client[i]);
        break;
      }
    }
    if (!added) {
      mylog("Too much clients, could not add %s", newClient.remoteIP().toString().c_str());
      newClient.stop();
    } 
  }

  // Handle disconnect
  for (size_t i = 0; i < MAX_CLIENTS; i++) {
    if (clientConnected[i] and !client[i].connected()) {
      onClientDisconnect(client[i]);
      clientConnected[i] = false;
    }
  }
}

void handleMQTTConnect() {
  if (WiFi.status() != WL_CONNECTED) return;
  if (mqttClient.connected()) {
    mqttClient.loop();
  } else {
    // No MQTT setting
    if (strlen(conf.myConf.mqttServer) < 3) return;
    if (mqttConnected) {
      mqttConnected = false;
      onMqttDisconnect();
      mqttTimer = millis() - MQTT_NEWTRY;
    }
    if ((long)(millis() - mqttTimer) < 0) return;
    mqttTimer += MQTT_NEWTRY;
    if ((long)(millis() - mqttTimer) < 0) mqttTimer = millis() + MQTT_NEWTRY; 
    bool conn = false;
    mqttClient.setServer(conf.myConf.mqttServer, conf.myConf.mqttPort);
    if (strlen(conf.myConf.mqttUser) > 1 && strlen(conf.myConf.mqttPwd) > 1) {
      conn = mqttClient.connect(conf.myConf.name, conf.myConf.mqttUser, conf.myConf.mqttPwd);
    } else {
      conn = mqttClient.connect(conf.myConf.name);
    }
    // MQTT Connection
    if (conn) {
      mqttConnected = true;
      onMqttConnect();
    } else {
      mylog("cannot connect to mqtt server %s", conf.myConf.mqttServer);
    }
  }
}





#define MAX_LOG_TEXT_LENGTH 500

char _logText[MAX_LOG_TEXT_LENGTH];

bool newLine = true;

char * timeStr() {
  return myTime.timeStr(true);
}

int _idx = 0;
void logflush() {
  _mylog(&_logText[0]);
  _idx = 0;
}

void logappend(char* _log, ...) {
  logappend((const char*)_log);
}

void logappend(const char* _log, ...) {
  if (_idx == 0) {
    _idx += snprintf(&_logText[_idx], MAX_LOG_TEXT_LENGTH, "%s: ", timeStr());
  }
  va_list args;
  va_start(args, _log);
  _idx += vsnprintf(&_logText[_idx], MAX_LOG_TEXT_LENGTH,  _log, args);
  va_end(args);
}

void mylog(char* _log, ...) {
  mylog((const char*)_log);
}

void mylog(const char* _log, ...) {
  if (_idx == 0) {
    _idx = snprintf(&_logText[0], MAX_LOG_TEXT_LENGTH, "%s: ", timeStr());
  }
  va_list args;
  va_start(args, _log);
  _idx += vsnprintf(&_logText[_idx], MAX_LOG_TEXT_LENGTH,  _log, args);
  va_end(args);
  _mylog(&_logText[0]);
  _idx = 0;
}

void _mylog(const char* _log) {
    // Has to be a one liner since data might be written from somewhere else
  Serial.println(_log);

  // Has to be a one liner since data might be written from somewhere else
  for (int i = 0; i < MAX_CLIENTS; i++) {
    if (client[i].connected()) client[i].println(_log);
  }
}
