#ifndef LORA_FUNCTIONS_H
#define LORA_FUNCTIONS_H

#include <Arduino.h>
#include <LoRa.h>
#include <HTTPClient.h>


void lora_rx();
void lora_setup();
void lora_send(String tx_data);


// Dichiara le funzioni usate e definite in main.cpp
void salvaConfig();
bool NTP_query();

// Dichiara funzioni usate in aprs.cpp
void aprsis_send(String aprsis_packet);
void calc_dist(String rxPacket,int pos2);

// Dichiara funzioni usate in ota_functions.cpp
void OTA_LOG();


// Dichiara come extern le variabili globali usate nella funzione

extern unsigned long millis_token;
extern bool token_tx;
extern String frequency;
extern String modulation;
extern String power;

extern String igate;
extern String digi;
extern String digi_radius;
extern String digi_route;
extern String digi_denied;
extern String igate_call;
extern String meteo_call;
extern String beacons_timing;
extern struct tm timeinfo;
extern String lastRX;
extern String dxRX;
extern unsigned long lastStIgBeacon;
extern String MHeard[42]; 
extern String beacons_switch;


#endif
