#ifndef OLED_FUNCTIONS_H
#define OLED_FUNCTIONS_H

#include <Adafruit_SSD1306.h>

void make_meteo_display();
void make_display();
void display_info(String info1, String info2);


// Dichiara come extern le variabili globali usate nella funzione

extern float TempC;
extern float Press;
extern float Hum;
extern String igate;
extern String igate_call;
extern String digi;
extern String telemetry;
extern String meteo;
extern String igate_backup;
extern String meteo_mode;

extern String myip;
extern String meteo_call;
extern Adafruit_SSD1306 display;

#endif
