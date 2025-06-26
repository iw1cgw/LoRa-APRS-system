#ifndef APRS_FUNCTIONS_H
#define APRS_FUNCTIONS_H

#include <Arduino.h>
#include <WiFi.h>
#include"haversine.h"


#define DESTCALL_digi    "APLHI0"      
#define DESTCALL_meteo   "APLHM0"


// Dichiara la funzione
void aprs_latlong();
void aprsis_send(String aprsis_packet);
void calc_dist(String rxPacket,int pos2);
void aprsis_setup();


String generateEncodedTelemetry();
String generateEncodedTelemetryBytes(int tempValue);

String getTempAPRS();
String getHumAPRS();
String getPressureAPRS();

void beacon_telemetry_param();
void beacon_igate_status();
void beacon_meteo_status();
void beacon_meteo();


void beacon_igate();







// Dichiara le funzioni usate e definite in main.cpp
void stop_bk();                 // dichiarazione della funzione che é in main.cpp
void lora_send(String tx_data);
void salvaConfig();
void start_sleep();
void wunder_send();



// Dichiara le variabili usate e definite in main.cpp o altrove ancora
extern String igate_call;
extern String igate_latlong;
extern String meteo_latlong;
extern int Km;
extern unsigned long aprs_login_millis;
extern bool aprs_login;
extern String aprs_server;
extern String aprs_passcode;

extern String meteo_call;
extern String aprs_sh;
extern String igate;
extern String digi;
extern String meteo;
extern String meteo_mode;
extern String fw;
extern String igate_backup;
extern bool backupStatus;
extern WiFiClient aprsis;  // dichiarazione della variabile globale

extern bool token_tx;
extern String telemetry;

extern byte pkRx10m; // pacchetti ricevuti da LoRa nei 10 minuti [ pacchetti validi ]
extern byte pkTx10m; // pacchetti trasmessi da LoRa nei 10 minuti [ come digipeater ]
extern byte pkig10m; // pacchetti gestiti da igate nei 10 minuti;
extern float voltage_dashboard;
extern int tlm_voltage;  // tensione 'aux' o partitore 4:1 oppure INA226 - valori per telemetria max 255
extern int tlm_current;  // corrente INA266 - valori per telemetria max 255
extern String igate_info;
extern String igate_beacon_arf;
extern String battery_view;

extern unsigned long lastIgBeacon;     // timer del beacon igate
extern unsigned long lastStIgBeacon;   // timer del beacon status dell'igate 
extern unsigned long lastStMtBeacon;   // timer del beacon status della meteo
extern unsigned long lastMtBeacon;     // timer del beacon meteo
extern unsigned long lastTlParamBeacon;// timer delle stringhe parametriche della telemetria

extern bool BM_sensor_status;
extern bool AHTstatus;
extern bool BMEstatus;
extern bool INA226_status;
extern String INA226_scale;

extern float TempC;      // valore corrente del termometro letto solo ogni 2000 millisecondi
extern float Hum;        // valore corrente del umidometro letto solo ogni 2000 millisecondi    
extern float Press;      // valore corrente del pressione letto solo ogni 2000 millisecondi 
extern double DewPoint;  // valore corrente del DewPoint calcolato solo ogni 2000 millisecondi 

extern String Zambretti; 
extern String meteo_info;
extern String beacons_timing;
extern String wunder_id;

extern byte cnt_telem;
extern String forecast;
extern String wunder_switch;

/*
extern String addGraphValue(String values, String value);
extern String generateGraph(String values, String graphName, String graphID, int r, int g, int b);
*/

extern String tempValues;
extern String pressValues;
extern String HumValues;
extern String fw;

#endif
