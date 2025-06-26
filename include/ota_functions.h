#ifndef OTA_FUNCTIONS_H
#define OTA_FUNCTIONS_H

#include <Arduino.h>
#include <WiFi.h>


//void OTA_log(String igatePacket);
void OTA_logbook();
bool NTP_query();
bool checkForUpdates();
bool updateFirmware();
String urlEncode(const String& str);


// Dichiara le funzioni usate e definite in main.cpp



// Dichiara funzioni usate in aprs.cpp


// Dichiara funzioni usate in oled.cpp
void OTA_display_ko();

// Dichiara come extern le variabili globali usate nella funzione

extern String gmt_zone;
extern bool RTC_status; 
extern String igate_call;
extern String meteo_call;
extern byte OTA_code;
extern String igate_mem;
extern String meteo;
extern String meteo_mode;
extern String frequency;
extern String power;
#endif
