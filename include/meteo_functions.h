#ifndef METEO_FUNCTIONS_H
#define METEO_FUNCTIONS_H

#include <Arduino.h>
#include <HTTPClient.h>
#include <aprs_functions.h>

void wunder_send();
void ZambrettiRoutine();


// Dichiara le funzioni usate e definite in aprs_functions.cpp


extern float TempC;      
extern float Hum;         
extern float Press;      
extern double DewPoint;  
extern String forecast;
extern String pressureTrend; 
extern String weatherSymbol;
extern float pressureTrendReference;
extern unsigned long pressureTrendTimeout;
extern bool BM_sensor_status;
extern bool AHTstatus;
extern bool BMEstatus;
extern bool INA226_status;
extern String wunder_id;
extern String wunder_key;
extern byte wunderSendStatus;

#endif