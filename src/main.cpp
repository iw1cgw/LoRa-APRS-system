#include <Arduino.h>
#include <ArduinoJson.h>
#include <LittleFS.h>
#include <Wire.h>
#include <WiFi.h> 
#include <HTTPClient.h>
#include <ESPAsyncWebServer.h>
#include <Adafruit_GFX.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_BME280.h>
#include <Adafruit_AHTX0.h>
#include <INA226_WE.h>   
#define I2C_ADDRESS 0x40
INA226_WE ina226 = INA226_WE(I2C_ADDRESS);

#include "config.h"
#include "main_page.h"
#include "setup_page.h"
#include "style.h"
#include "oled_functions.h"
#include "aprs_functions.h"
#include "lora_functions.h"
#include "rtc.h"
#include "ota_functions.h"
#include "meteo_functions.h"


//---------------------------------------------------------------------------------------------- variabili JSON
String igate_call = "N0CALL-10";
String igate_latlong = "44.384470,7.779091";                                  
String igate_info = "... APRS iGate LoRa tech ...";

String beacons_switch = "ko";
String beacons_timing = "10";
String igate_beacon_arf = "ko";     // send iGate beacon also in RF is ON

String igate_backup = "ok";
String digi_radius = "30";
String digi_route = "";
String digi_denied = "";

String meteo_call = "N0CALL-13";
String meteo_latlong = "44.384470,7.779091";
String meteo_info = "... APRS Meteostation LoRa tech ...";
String meteo_altitude = "0"; 
String drift_temperature = "0"; 
String drift_pressure = "0";
String Zambretti = "ko";

String frequency = "433.775";
String power = "17";
String modulation = "300";
String WiFi1_ssid = "";
String WiFi1_pwd = "";
String WiFi2_ssid = "";
String WiFi2_pwd = "";
String access_point_pwd = "00000000";
String access_point_asd = "ko";               // access point auto shut-down
String aprs_server = "rotate.aprs2.net";
String aprs_passcode = "";
String aprs_sh = "ko";                        // aprs connection server hide [stealth]
String wunder_switch = "ko";
String wunder_id = "";
String wunder_key = "";
String password = "admin";
String gmt_zone = "+1";
String battery_view = "ko";         // abilita o meno la vista del dato di batteria sui beacon APRS
String drift_battery = "";          // valore per fine trimmatura valore batteria del TTGO

String oled = "ok";
String INA226_scale = "1";   //       current_mA = ina226.getCurrent_mA();  1 / 10     if ( INA226_scale == "10" ) current_mA = current_mA *10;

String igate = "ok";        // se ok indica igate abilitato
String digi = "ko";         // se ok indica digipeater abilitato
String meteo = "ko";        // se ok indica meteo abilitato
String meteo_mode = "rf";   // rf / ip / rf_sleep //
String telemetry = "ko";    // se ok indica telemetria abilitata

String cnt_telemS = "0";    // progressivo contatore pacchetti telemetria formato string json


//---------------------------------------------------------------------------------------------- SYSTEM
String fw = Release;
String tmp_dash_pwd = "admin";
//char imp[121]="";                   // a supporto caricamento EEPROM per dati legacy della 1.02 
char car;                             // a supporto seriale
String myip = "0.0.0.0";              // indicazione ip corrente
bool backupStatus;                    // backup ip/rf attivo/non attivo
String igate_mem;                     // a supporto operazioni di backup per perdita connessione WiFi
String digi_mem;                      // a supporto operazioni di backup per perdita connessione WiFi
String meteo_mode_mem;                // a supporto operazioni di backup per perdita connessione WiFi
bool AP_active;                       // indicazione se Access Point attivo/non attivo
unsigned long lost_connection_millis; // timer verifica perdita connessione WiFi
unsigned long timeout_millis;         // timer timeout per spegnimento AP e scadenza login
int oled_switch;                      // contatore 0/3 per lo switch delle informazioni sul display OLED
bool mem_oled = HIGH;                 // a supporto del sistema di switch delle informazioni sul display OLED
byte OTA_code;                        // codici controllo OTA
int retr;


//---------------------------------------------------------------------------------------------- INA226
boolean INA226_status = LOW;          // indica INA226 rilevato o meno allo startup
float shuntVoltage_mV = 0.0;          // INA226 varibili correlate
float loadVoltage_V = 0.0;            // INA226 varibili correlate
float busVoltage_V = 0.0;             // INA226 varibili correlate
float current_mA = 0.0;               // INA226 varibili correlate corrente puntuale in Ampere


//---------------------------------------------------------------------------------------------- telemetrie & misure
float voltage = 0;
float voltage_aux = 0;        // tensione AUX rilevata da INA226
float battPercent = 0;
float voltage_dashboard = 0;  // tensione puntuale in volt della batteria accessoria del TTGO
int tlm_voltage = 0;          // valori parametrizzati 0-255 per telemetria - tensione 'aux' INA226
int tlm_current = 0;          // valori parametrizzati 0-255 per telemetria - corrente 'aux' INA226
byte cnt_telem;               // progressivo contatore pacchetti telemetria formato byte


//---------------------------------------------------------------------------------------------- APRS beacon
unsigned long aprs_login_millis;      // timer verifica bontà connession aprs-is a supporto operazioni di backup per perdita connessione WiFi
String current_server_aprsis;         // indicazione del server aprs-is connesso
bool aprs_login;                      // login aprs-is attivo/non attivo
unsigned long lastIgBeacon = 0;       // timer del beacon igate
unsigned long lastStIgBeacon = 0;     // timer del beacon status dell'igate 
unsigned long lastStMtBeacon = 0;     // timer del beacon status della meteo
unsigned long lastMtBeacon = 0;       // timer del beacon meteo
unsigned long lastTlParamBeacon = 0;  // timer delle stringhe parametriche della telemetria


//---------------------------------------------------------------------------------------------- deep sleep e RTC
boolean switch_sleep = false;
RTC_DATA_ATTR int bootCount = 0;
bool RTC_status;        //struct tm timeinfo;     // vedi rtc.h per estensione a moduli


//---------------------------------------------------------------------------------------------- Meteo
Adafruit_BME280 bme;
Adafruit_BMP280 bmp;
Adafruit_AHTX0 aht;
bool BM_sensor_status = false;
bool AHTstatus = false;
bool BMPstatus = false;
bool BMEstatus = false;
String forecast;
String pressureTrend; 
String weatherSymbol;
unsigned long pressureTrendTimeout;
float pressureTrendReference = 0;
float TempC;      // valore corrente del termometro
float Hum;        // valore corrente del umidometro 
float Press;      // valore corrente del pressione
double DewPoint;  // valore corrente del DewPoint
Adafruit_Sensor *bme_temp = bme.getTemperatureSensor();   // Adafruit_BME280 bme; // use I2C interface
Adafruit_Sensor *bme_pressure = bme.getPressureSensor();  // Adafruit_BME280 bme; // use I2C interface
Adafruit_Sensor *bme_humidity = bme.getHumiditySensor();  // Adafruit_BME280 bme; // use I2C interface
sensors_event_t humidity_event, temp_event, pressure_event;
byte wunderSendStatus=0;          // 0 indefinito - 1 ok - 9 ko
String Zambretti_enabled = "ok";  // abilita per le stazioni italiane la funzionalità 'Zambretti'


//---------------------------------------------------------------------------------------------- LoRa
String lastRX;
String dxRX;
String MHeard[42];          // matrice delle indicazione delle ultime 40 stazioni ascoltate
unsigned long millis_token; // contatore del token di abilitazione tx LoRa
bool token_tx;              // token di abilitazione tx LoRa


//---------------------------------------------------------------------------------------------- SSD1306 OLED
#define SSD1306_ADDRESS 0x3C
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET 23 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
const unsigned char logo [] PROGMEM = {
// '128X64', 128x64px
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0x80, 0x00, 0x03, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xfe, 0x07, 0xff, 0x81, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xfe, 0x1f, 0xff, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0x7e, 0x01, 0xfd, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xc0, 0xfe, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xc7, 0xff, 0xc7, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xf0, 0x7f, 0xff, 0xff, 0xfe, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xe0, 0x7f, 0xfe, 0x01, 0xfc, 0x00, 0x03, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xe0, 0x7f, 0xf8, 0x10, 0x3c, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xe0, 0x7f, 0xfb, 0xff, 0x3c, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xe0, 0x7f, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xe0, 0x7f, 0xff, 0x83, 0xfc, 0x0f, 0xe0, 0x7f, 0x07, 0x89, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xe0, 0x7f, 0xfc, 0x00, 0xfc, 0x0f, 0xe0, 0x78, 0x00, 0x81, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xe0, 0x7f, 0xf8, 0x00, 0x3c, 0x0f, 0xe0, 0x70, 0x00, 0x51, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xe0, 0x7f, 0xf0, 0x00, 0x3c, 0x0f, 0xe0, 0x70, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xe0, 0x7f, 0xf0, 0x30, 0x1c, 0x0f, 0xc0, 0x70, 0x70, 0x3f, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xe0, 0x7f, 0xe0, 0x7c, 0x1c, 0x00, 0x00, 0x70, 0xf8, 0x3f, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xe0, 0x7f, 0xe0, 0xfc, 0x0c, 0x00, 0x00, 0xff, 0xc0, 0x3f, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xe0, 0x7f, 0xe0, 0xfe, 0x0c, 0x00, 0x01, 0xfc, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xe0, 0x7f, 0xe0, 0xfe, 0x0c, 0x00, 0x07, 0xf8, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xe0, 0x7f, 0xe0, 0xfe, 0x0c, 0x06, 0x03, 0xf0, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xe0, 0x7f, 0xe0, 0xfe, 0x0c, 0x0f, 0x03, 0xe0, 0x70, 0x3f, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xe0, 0x7f, 0xe0, 0xfc, 0x0c, 0x0f, 0x01, 0xe0, 0xf0, 0x3f, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xe0, 0x00, 0x20, 0x7c, 0x1c, 0x0f, 0x81, 0xe0, 0xf0, 0x3f, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xe0, 0x00, 0x30, 0x00, 0x1c, 0x0f, 0x80, 0xe0, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xe0, 0x00, 0x30, 0x00, 0x3c, 0x0f, 0xc0, 0xe0, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xe0, 0x00, 0x38, 0x00, 0x7c, 0x0f, 0xe0, 0x70, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xf0, 0x00, 0x7e, 0x00, 0xfe, 0x0f, 0xe0, 0x78, 0x08, 0x3f, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xf9, 0xff, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0x83, 0xff, 0xfc, 0xff, 0xfa, 0x66, 0xef, 0x1e, 0x3b, 0xbb, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xcf, 0xff, 0xef, 0xfc, 0xff, 0xf2, 0x44, 0xce, 0x0c, 0x09, 0x13, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xc3, 0xff, 0x87, 0xfc, 0x26, 0x72, 0x44, 0x8c, 0xc8, 0xc9, 0x17, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xe0, 0x38, 0x0f, 0xfc, 0x26, 0x72, 0x01, 0x0c, 0xf9, 0xf8, 0x07, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x3f, 0xf9, 0x94, 0xf2, 0x01, 0xcc, 0xf9, 0x08, 0x8f, 0xff, 
0xff, 0xff, 0xff, 0xfe, 0x7f, 0x83, 0xf9, 0xf9, 0x91, 0xf2, 0x23, 0xcc, 0xc9, 0x18, 0x8f, 0xff, 
0xff, 0xff, 0xff, 0xfe, 0x0f, 0xff, 0xe0, 0xf8, 0x31, 0xe6, 0x23, 0x9c, 0x08, 0x18, 0x9f, 0xff, 
0xff, 0xff, 0xff, 0xff, 0x01, 0xff, 0x01, 0xf8, 0x73, 0xe6, 0x67, 0x9e, 0x3c, 0x39, 0x9f, 0xff, 
0xff, 0xff, 0xff, 0xff, 0x80, 0x00, 0x07, 0xff, 0xe3, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x1f, 0xff, 0xc7, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

//==================================================================================================

WiFiClient aprsis;
WiFiClient wunder;

AsyncWebServer server(80);

// Funzione per caricare da JSON
void caricaConfig() {
  if (!LittleFS.exists("/config.json")) {
    Serial.println("File config non trovato");
    return;
  }

  File file = LittleFS.open("/config.json", "r");

  if (file) {
    JsonDocument doc;

    DeserializationError error = deserializeJson(doc, file);
    if (!error) {

      igate_call = doc["igate_call"].as<String>();
      igate_latlong = doc["igate_latlong"].as<String>();
      igate_info = doc["igate_info"].as<String>();
      beacons_switch = doc["beacons_switch"].as<String>();
      beacons_timing = doc["beacons_timing"].as<String>();
      igate_beacon_arf = doc["igate_beacon_arf"].as<String>();
      igate_backup = doc["igate_backup"].as<String>();
      digi_radius = doc["digi_radius"].as<String>();
      digi_route = doc["digi_route"].as<String>();
      digi_denied = doc["digi_denied"].as<String>();

      meteo_call = doc["meteo_call"].as<String>();
      meteo_latlong = doc["meteo_latlong"].as<String>();
      meteo_info = doc["meteo_info"].as<String>();
      meteo_altitude = doc["meteo_altitude"].as<String>();
      drift_temperature = doc["drift_temperature"].as<String>();
      drift_pressure = doc["drift_pressure"].as<String>();
      Zambretti = doc["Zambretti"].as<String>();;

      frequency = doc["frequency"].as<String>();
      power = doc["power"].as<String>();
      modulation = doc["modulation"].as<String>();

      WiFi1_ssid = doc["WiFi1_ssid"].as<String>();
      WiFi1_pwd = doc["WiFi1_pwd"].as<String>();
      WiFi2_ssid = doc["WiFi2_ssid"].as<String>();
      WiFi2_pwd = doc["WiFi2_pwd"].as<String>();
      access_point_pwd = doc["access_point_pwd"].as<String>();
      access_point_asd = doc["access_point_asd"].as<String>();
      aprs_server = doc["aprs_server"].as<String>();
      aprs_passcode = doc["aprs_passcode"].as<String>();
      aprs_sh = doc["aprs_sh"].as<String>();
      wunder_switch = doc["wunder_switch"].as<String>();
      wunder_id = doc["wunder_id"].as<String>();
      wunder_key = doc["wunder_key"].as<String>();
      password = doc["password"].as<String>();
      gmt_zone = doc["gmt_zone"].as<String>();
      battery_view = doc["battery_view"].as<String>();
      drift_battery = doc["drift_battery"].as<String>();
      oled = doc["oled"].as<String>();
      INA226_scale = doc["INA226_scale"].as<String>();

      igate = doc["igate"].as<String>();
      digi = doc["digi"].as<String>();
      telemetry = doc["telemetry"].as<String>();
      meteo = doc["meteo"].as<String>();
      meteo_mode = doc["meteo_mode"].as<String>();

      cnt_telemS = doc["cnt_telemS"].as<String>();

      Serial.println("Configuration loaded");

    } else {
      Serial.println("Errore nel parsing JSON");
    }
    file.close();
  }
}



String ipToString(IPAddress ip){
  String s="";
  for (int i=0; i<4; i++)
    s += i  ? "." + String(ip[i]) : String(ip[i]);
  
    return s;
}



void verifica_parametri() {

  aprs_latlong(); // riestrai dati corretti 

  meteo_call.toUpperCase();  
  igate_call.toUpperCase();  
  digi_denied.toUpperCase();  
  digi_route.toUpperCase();  
     
  if (digi_radius.toInt() >30 ) digi_radius = "30";
  if (digi_radius.toInt() == 0 ) digi_radius = "1";

  if ( meteo == "ok" && meteo_mode == "rf_sleep" ) {
    digi = "ko";
    igate = "ko";
  }

  if ( !BM_sensor_status ) meteo = "ko";
  if ( digi == "ok" && igate == "ok" ) digi = "ko";
  if ( digi != "ok" && igate != "ok" && meteo != "ok" ) telemetry = "ko";

  if (beacons_timing.toInt() < 10 ) beacons_timing = "10";
  
  if (power.toInt() >20 ) power = "17";

  if (drift_temperature.toInt() >30 ) drift_temperature = "0"; 
  if (drift_pressure.toInt() >30 ) drift_pressure = "0"; 

    if (meteo_call.substring(0, 1) != "I" ) {
    Zambretti_enabled = "ko"; // non consentire 'Zambretti' per stazioni non italiane - blocca uso toggle sulla setup
    Zambretti = "ko";         // non consentire 'Zambretti' per stazioni non italiane - nascondi riga forecast sulla dashboard
  }
  else Zambretti_enabled = "ok";  

  //Serial.println("Configuration verified");
}



void salvaConfig() {
  
  verifica_parametri();
   
  JsonDocument doc;                   //StaticJsonDocument<512> doc;

  doc["igate_call"] = igate_call;
  doc["igate_latlong"] = igate_latlong;
  doc["igate_info"] = igate_info;

  doc["igate_backup"] = igate_backup;
  doc["digi_radius"] = digi_radius;
  doc["digi_route"] = digi_route;
  doc["digi_denied"] = digi_denied;
  doc["meteo_call"] = meteo_call;
  doc["meteo_latlong"] = meteo_latlong;
  doc["meteo_info"] = meteo_info;

  doc["meteo_altitude"] = meteo_altitude;
  doc["drift_temperature"] = drift_temperature;
  doc["drift_pressure"] = drift_pressure;
  doc["Zambretti"] = Zambretti;

  doc["beacons_switch"] = beacons_switch;
  doc["beacons_timing"] = beacons_timing;
  doc["igate_beacon_arf"] = igate_beacon_arf;

  doc["frequency"] = frequency;
  doc["power"] = power;
  doc["modulation"] = modulation;

  doc["WiFi1_ssid"] = WiFi1_ssid;
  doc["WiFi1_pwd" ] = WiFi1_pwd;
  doc["WiFi2_ssid"] = WiFi2_ssid;
  doc["WiFi2_pwd"] = WiFi2_pwd;
  doc["access_point_pwd"] = access_point_pwd;
  doc["access_point_asd"] = access_point_asd;
  doc["aprs_server"] = aprs_server;
  doc["aprs_passcode"] = aprs_passcode;
  doc["aprs_sh"] =  aprs_sh;
  doc["wunder_switch"] = wunder_switch;
  doc["wunder_id"] = wunder_id;
  doc["wunder_key"] = wunder_key;

  doc["password"] = password;
  doc["gmt_zone"] = gmt_zone;
  doc["drift_battery"] = drift_battery;
  doc["battery_view"] = battery_view;
  doc["oled"] = oled;
  doc["INA226_scale"] = INA226_scale;

  doc["igate"] = igate;
  doc["digi"] = digi;
  doc["telemetry"] = telemetry;
  doc["meteo"] = meteo;
  doc["meteo_mode"] = meteo_mode;
  doc["cnt_telemS"] = String(cnt_telem);

  File file = LittleFS.open("/config.json", "w");
  if (file) {
    serializeJson(doc, file);
    file.close();
    //Serial.println("Configurazione salvata");
  } else {
    Serial.println("Errore apertura file per scrittura");
  }
}



void setup_WiFi( String WiFi_ssiD, String WiFi_pwd ) {

    WiFi.setAutoReconnect(true);
    WiFi.begin(WiFi_ssiD, WiFi_pwd);      // Connessione alla rete WiFi esistente (Station) se non si é in sleep mode
    Serial.print("Connecting to WiFi: " );Serial.print(WiFi_ssiD);
    retr = 0 ;
    while (retr <= 10 ) {
      retr++;
      Serial.print(".");
      delay(1000);
      if ( WiFi.status() == WL_CONNECTED ) retr = 21;
    }

    if ( WiFi.status() == WL_CONNECTED ) {
      Serial.print("\nConnected, IP: ");
      Serial.println(WiFi.localIP());
      myip = ipToString(WiFi.localIP());
    } 
    else {
      Serial.println("\nerror connecting to wifi ");
      //myip = "0.0.0.0";
    } 
}  


// --- attiva il backup dell' iGate per fault WiFi o APRS-FI
void start_bk() {
  Serial.println(".. backup igate/digi actived..");
    meteo_mode_mem = meteo_mode;
    igate_mem = igate;
    digi_mem = digi;
  
  if ( igate == "ok" ) {
      digi = "ok";
      igate = "ko";
    }
    
    if ( meteo_mode == "ip" ) meteo_mode = "rf";
    backupStatus = true;
    aprs_login = false;
  }



// --- ripristina il setup ante esecuzione del backup
void stop_bk() { 
  Serial.println(".. backup igate/digi deactivated ..");
  igate = igate_mem;               //--- ripristina il setup ante esecuzione del backupricarica i valori previsti
  digi = digi_mem; 
  meteo_mode = meteo_mode_mem;  
  backupStatus = false;
  aprs_login = false;
}



float getTempC() {
  if (BM_sensor_status || AHTstatus ) {
    if ( BMEstatus ) {
      //sensors_event_t temp_event, pressure_event, humidity_event;
      bme_temp->getEvent(&temp_event);
      return temp_event.temperature;  // use BME280
    }
    if ( AHTstatus ) {                // use AHT20  
      //sensors_event_t humidity_event, temp_event;
      aht.getEvent(&humidity_event, &temp_event);
      return temp_event.temperature;
    }
    if ( BMPstatus ) {
      bme_temp->getEvent(&temp_event);
      return bmp.readTemperature();  // use BMP280
    }
  } return 0;
}  
  
float getHum() {
  if ( AHTstatus || BMEstatus ) {
    if(AHTstatus) {
      aht.getEvent(&humidity_event, &temp_event);
      return humidity_event.relative_humidity;
    }
    if(BMEstatus) {
      bme_humidity->getEvent(&humidity_event);
      return humidity_event.relative_humidity;
    }
  }
  return 0; 
}

float getDewPoint() {
  // Calculate dew Point
  double A0= 373.15/(273.15 + TempC);
  double SUM = -7.90298 * (A0-1);
  SUM += 5.02808 * log10(A0);
  SUM += -1.3816e-7 * (pow(10, (11.344*(1-1/A0)))-1) ;
  SUM += 8.1328e-3 * (pow(10,(-3.49149*(A0-1)))-1) ;
  SUM += log10(1013.246);
  double VP = pow(10, SUM-3) * Hum;
  double T = log(VP/0.61078);   
  DewPoint = (241.88 * T) / (17.558-T);
  return DewPoint;
}

float getPressure(float TempC) {
  if (BM_sensor_status) {
    float Tmp_read_pressure;
    if(BMPstatus) Tmp_read_pressure = bmp.readPressure()/100;
    if(BMEstatus) {
      bme_pressure->getEvent(&pressure_event);
      Tmp_read_pressure = pressure_event.pressure;
    }
    /*  // ------------- formula che tiene conto della temperatura 
        //SLM_pressure = pres_BMP280 * ( pow(1.0 - (0.0065 * (float) Elevation * -1 / (273.15 + temp_Celsius )), 5.255));


        // ------------- formula Bosch che NON tiene conto della temperatura 
        SLM_pressure = (((pres_BMP280)/pow((1-((float)(Elevation))/44330), 5.255))/1);
    */

    if (meteo_altitude.toInt() <= 400) return drift_pressure.toInt() + Tmp_read_pressure * ( pow(1.0 - (0.0065 * meteo_altitude.toFloat() * -1 / (273.15 + TempC )), 5.255));
    if (meteo_altitude.toInt() >  400) return drift_pressure.toInt() + ((Tmp_read_pressure/pow((1-( meteo_altitude.toFloat() )/44330), 5.255))/1);

  }   
  return 0;
}   



void start_sleep() { 
    LoRa.sleep();
    Serial.println("LoRa RTX sleep");
    delay(350);
    delay(350);
    WiFi.disconnect(true); // Disabilita WiFi e cancella credenziali salvate
    delay(350);
    WiFi.mode(WIFI_OFF);   // Spegne il modulo WiFi
    Serial.println("WiFi module sleep");
    /*
    il semplice sleep del TTGO porta il consumo in sleep a circa 5 mA
    spegnendo tutti i sensori e il chip RTX LoRa il consumo scende ulteriormente a soli 1.8 mA/h    
    lo spegnimento del display OLED anche nel normale funzionamento non fa scender ein modo significativo il consumo che é di circa 
    153 mA per il funzionamento in stand-by che salgono a 260 mA durante la trasmissione al più alto livello di potenza
    */


    
    if ( BMPstatus ) { 
      bmp.setSampling(Adafruit_BMP280::MODE_SLEEP);
      Serial.println("BMP280 sleep");
    }

    if ( AHTstatus ) {
      Wire.beginTransmission(0x38);
      Wire.write(0xB0);  // Comando per entrare in sospensione
      Wire.endTransmission();
      usleep(2000);  // Attendi almeno 1 ms per assicurarsi che il comando sia processato
      Serial.println("AHT20 sleep");
  }

    if ( BMEstatus ) { 
      Wire.beginTransmission(0x76);
      Wire.write(0xF4);  // Registro di controllo
      Wire.write(0x00);  // Modalità sospensione (sleep mode)
      Wire.endTransmission();
      usleep(2000);  // Attendi almeno 1 ms per assicurarsi che il comando sia processato
      Serial.println("BME280 sleep");
    }

    if ( INA226_status ) { 
      Wire.beginTransmission(0x40);
      Wire.write(0x00);
      Wire.write(0x00);  // Byte alto (MSB)
      Wire.write(0x00);  // Byte basso (LSB)
      Wire.endTransmission();
      Serial.println("INA226 sleep");
    }
 
    display.dim(true);
    display.display();
    esp_sleep_enable_timer_wakeup(beacons_timing.toInt() * 60 * uS_TO_S_FACTOR);
    Serial.println("Setup ESP32 to sleep for every " + String(beacons_timing.toInt() * 60) + " Seconds");
    Serial.println("Going to deep sleep now");
    delay(1000);
    Serial.flush(); 
    esp_deep_sleep_start();
    Serial.println("This will never be printed");
  }



  bool check_pwd() { 
   
    if ( password == tmp_dash_pwd || tmp_dash_pwd == "scodella" ) { 
      timeout_millis = millis();
      return true;
    }
    else return false;
  }


void setup() {
  
  pinMode (PLed_life_int, OUTPUT);
  //pinMode(V12_SENSOR_PIN  , INPUT);
  pinMode(SS, OUTPUT);
  digitalWrite(SS, HIGH);

  Serial.begin(115200);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3c); 
  display.clearDisplay();
  display.drawBitmap(0, 0, logo, 128, 64, WHITE);
  display.display();
  delay(2000);
    
  //--------------------------------------------------------------------------------------
  BM_sensor_status = false;
  BMPstatus = false;
  BMEstatus = false;
  AHTstatus = false;

  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  int tmp=9;
   //------------------------------ test del AHT20
  if (!aht.begin()) {}
  else
    {
      Serial.println("AHT20 0x38 OK");
      AHTstatus = true;
      display.setCursor(0,tmp);
      display.print("sensor AHT20 0x38 OK");
      tmp=tmp+9;
    } 
  
  if ( AHTstatus ) {
    Wire.beginTransmission(0x38);
    Wire.write(0xA8);  // Comando per uscire dalla sospensione
    Wire.endTransmission();
    usleep(10000);  // Attendi almeno 10 ms per stabilizzazione
  } 

  //------------------------------ test del BME280
  if (AHTstatus == false) {
    if (!bme.begin(0x76)) {}
    else {
      BMEstatus = true;
      BM_sensor_status = true;
      Serial.println("BME280 0x76 OK");
      display.setCursor(0,tmp);
      display.print("sensor BME280 0x76 OK");
      tmp=tmp+9;
    }
  }

  //------------------------------ test del BMP280 su 2 possibili indirizzi 0x77 se integrato con AHT20 o 0x76 se stand alone

  if (AHTstatus == true && BMEstatus == false) {
    if (!bmp.begin(0x77)) {}
    else {
      Serial.println("BMP280 0x77 OK");
      BMPstatus = true;
      BM_sensor_status = true;
      display.setCursor(0,tmp);
      display.print("sensor BMP280 0x77 OK");
      tmp=tmp+9;
    }
  }
 
  if (AHTstatus == false && BMEstatus == false) {
    if (!bmp.begin(0x76)) {}
    else {
      Serial.println("BMP280 0x76 OK");
      BMPstatus = true;
      BM_sensor_status = true;
      display.setCursor(0,tmp);
      display.print("sensor BMP280 0x76 OK");
      tmp=tmp+9;
   }
  }


  //------------------------------  risoluzione conflitti per sensori multipli
  if ( BMEstatus) {
    BMPstatus = LOW;
    AHTstatus = LOW;
  }
  //------------------------------  risoluzione conflitti per sensori multipli 

  
//------------------------------ test del INA226

  if ( !ina226.init() ) {}
  else {
    Serial.println(F("INA226 0x40 OK"));
    INA226_status = HIGH; 
    display.setCursor(0,tmp);
    display.print("sensor INA226 0x40 OK");
    tmp=tmp+9;
    ina226.setResistorRange(0.1, 1.3); // choose resistor 0.1 Ohm and gain range up to 1.3A
  
    /* If the current values delivered by the INA226 differ by a constant factor
      from values obtained with calibrated equipment you can define a correction factor.
      Correction factor = current delivered from calibrated equipment / current delivered by INA226
    */ 
  
    ina226.setCorrectionFactor(0.93);
    ina226.waitUntilConversionCompleted(); //if you comment this line the first data might be zero
  } 


  // Avvia LittleFS
if (!LittleFS.begin()) {
  Serial.println("Errore inizializzazione LittleFS. Provo a formattare...");
  LittleFS.format();  // FORZA la formattazione
  if (LittleFS.begin()) {
    Serial.println("LittleFS formattato e montato con successo.");
  } else {
    Serial.println("Fallita anche la formattazione.");
    return;
  }
}

//============================================================================================

 
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html);
  });


  server.on("/setup", HTTP_GET, [](AsyncWebServerRequest *request) {
    timeout_millis = millis();                         //--- reset contatore attività
    if ( check_pwd() || password == "admin" ) request->send(200, "text/html", setup_html);
    else request->redirect("/login");
  });


  //--------------------------------------------------------------------- richiesta password
  server.on("/login", HTTP_GET, [](AsyncWebServerRequest *request){
    String html = R"rawliteral(
      <html>
        <body>
          <script>
            let pwd = prompt("Enter password", "");
            if (pwd !== null) {
              window.location.href = "/new-value/" + encodeURIComponent(pwd);
            } else {
              // L'utente ha premuto Annulla
              window.location.href = "/";
            }
          </script>
        </body>
      </html>
    )rawliteral";
    request->send(200, "text/html", html);
  });
  //--------------------------------------------------------------------- richiesta password



  //--------------------------------------------------------------------- recupero valore password richiesta 

  server.on("/new-value/*", HTTP_GET, [](AsyncWebServerRequest *request){
  String fullPath = request->url(); // ad es. "/new-value/password123"
  tmp_dash_pwd = fullPath.substring(String("/new-value/").length());
  timeout_millis = millis();                               // fai partire il contatore di timeout
  request->redirect("/");
  });
  //--------------------------------------------------------------------- recupero valore password richiesta 
  


  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/css", style_css);  // stile servito come file
  });


  server.on("/reboot", HTTP_GET, [](AsyncWebServerRequest *request){
    igate_mem = "reboot";   // utilizzo termporaneo improprio di igate_mem per eseguire un ESP.restart() differito
    request->redirect("/");
  });


//--------------------------------------------------------------------------------------------- pagina MH
  server.on("/mh", HTTP_GET, [](AsyncWebServerRequest *request){
 
  //<head>
  
  String html = "<!DOCTYPE html><html><head><meta charset='UTF-8'><title>MH</title>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<link rel='stylesheet' href='/style.css'>";
  html += "<fieldset><legend>received stations</legend>";
  //html += "<h1 style='text-align:left;line-height: 1.0; font-weight: bold; font-size: 9pt'>";
  html += "<h1 style='line-height: 1.0; font-size: 9pt'>";

    for (retr = 0; retr <= 40; retr++) {
      html += MHeard[retr] + "<br>";
    }

  html += "<form action='/'><input type='submit' value='return' style='float:right;'></form>";
  html += "</h1></fieldset></body></html>";

    request->send(200, "text/html", html);
  });
//--------------------------------------------------------------------------------------------- pagina MH

  server.on("/beacon", HTTP_GET, [](AsyncWebServerRequest *request) {
  
    if ( check_pwd() || password == "admin" ) {
        if ( meteo == "ok" || digi == "ok" || igate == "ok" || wunder_switch == "ok" ) {
          lastMtBeacon = millis() - beacons_timing.toInt() * 60000;
          lastStMtBeacon = millis() - beacons_timing.toInt()*24 * 60000;
          lastIgBeacon = millis() - beacons_timing.toInt() * 60000;
          lastStIgBeacon = millis() - beacons_timing.toInt()*24 * 60000;
          request->redirect("/");
        }
        else {

        } 
      } 
    else request->redirect("/login");
  });


  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/css", style_css);
  });


  // Endpoint POST per mandare i valori di configurazione alla main page HTML
  server.on("/dash_value", HTTP_POST, [](AsyncWebServerRequest *request){

    String Tmp = "IW1CGW-10"; 
    if ( meteo == "ok" ) Tmp = meteo_call;    
    if ( igate == "ok" || digi == "ok" ) Tmp = igate_call;
    String Tmp2;
    if ( AHTstatus && BM_sensor_status ) Tmp2 = "04";
    if ( BMPstatus && !AHTstatus ) Tmp2 = "02";
    
    String json = "{";
    json += "\"igate\":\"" + igate + "\",";
    json += "\"digi\":\"" + digi + "\",";
    json += "\"telemetry\":\"" + telemetry + "\",";
    json += "\"BM_sensor_status\":\"" + String(BM_sensor_status ? "ok" : "ko") + "\",";
    json += "\"INA226_status\":\"" + String(INA226_status ? "ok" : "ko") + "\",";
    json += "\"type_grid\":\"" +Tmp2 + "\",";
    json += "\"meteo\":\"" + meteo + "\",";
    json += "\"meteo_mode\":\"" + meteo_mode + "\",";
    json += "\"battery_view\":\"" + battery_view + "\",";
    json += "\"Zambretti\":\"" + Zambretti + "\",";                   // serve solo per abilitare la visualizzazione della forecast
    json += "\"beacons_switch\":\"" + beacons_switch + "\",";
    json += "\"call\":\"" + igate_call.substring(0, igate_call.indexOf('-')) + "\",";
    json += "\"aprs_view_call\":\"" + Tmp + "\",";                                          // valore per visualizzare APRS cliccando sulla label iniziale
    json += "\"fw\":\"" + fw + "\"";
    json += "}";
    //Serial.println(json);
    request->send(200, "application/json", json);
  });


// Endpoint POST per mandare i valori di dinamici ciclici alla main page HTML
  server.on("/dash_meteo_value", HTTP_POST, [](AsyncWebServerRequest *request){
      
    String json = "{";
    json += "\"TempC\":\"" + String(TempC,1) + "\",";
    json += "\"Press\":\"" + String(Press,1) + "\",";
    json += "\"Hum\":\"" + String(Hum,1) + "\",";
    json += "\"DewPoint\":\"" + String(DewPoint,1) + "\",";
    json += "\"battPercent\":\"" + String(battPercent,1) + "\",";
    json += "\"voltage_dashboard\":\"" + String(voltage_dashboard,1) + "\",";
    json += "\"aux_voltage\":\"" + String(loadVoltage_V,1) + "\",";
    json += "\"aux_current\":\"" + String(current_mA,1) + "\",";
    json += "\"current_server_aprsis\":\"" + current_server_aprsis + "\",";
    json += "\"lastRX\":\"" + lastRX + "\",";
    json += "\"dxRX\":\"" + dxRX + "\",";
    json += "\"myip\":\"" + myip + "\",";
    json += "\"pressureTrend\":\"" + pressureTrend + "\",";
    json += "\"forecast\":\"" + forecast + "\"";
    json += "}";
    //Serial.println(json);
    request->send(200, "application/json", json);
  });



// Riceve dal form della dashboard e salva i dati 
  server.on("/dash_save", HTTP_POST, [](AsyncWebServerRequest *request) {
    if ( check_pwd() || password == "admin" ) {
      if (request->hasParam("igate", true)) {
        igate = request->getParam("igate", true)->value();
      }
      if (request->hasParam("digi", true)) {
        digi = request->getParam("digi", true)->value();
      }
      if (request->hasParam("telemetry", true)) {
        telemetry = request->getParam("telemetry", true)->value();
      }
      if (request->hasParam("meteo", true)) {
        meteo = request->getParam("meteo", true)->value();
      }
      if (request->hasParam("meteo_mode", true)) {
        meteo_mode = request->getParam("meteo_mode", true)->value();
      }
      salvaConfig();
      request->redirect("/");
    }
    else request->redirect("/login");
  });


// Endpoint POST per inviare i valori attuali nella setup che li recupera in fetch
  server.on("/valori", HTTP_POST, [](AsyncWebServerRequest *request){
    
    String json = "{";
    json += "\"igate_call\":\"" + igate_call + "\",";
    json += "\"igate_latlong\":\"" + igate_latlong + "\",";
    json += "\"igate_info\":\"" + igate_info + "\",";
    json += "\"igate_backup\":\"" + igate_backup + "\",";
    json += "\"digi_radius\":\"" + digi_radius + "\",";
    json += "\"digi_route\":\"" + digi_route + "\",";
    json += "\"digi_denied\":\"" + digi_denied + "\",";
    json += "\"meteo_call\":\"" + meteo_call + "\",";
    json += "\"meteo_latlong\":\"" + meteo_latlong + "\",";
    json += "\"meteo_info\":\"" + meteo_info + "\",";

    json += "\"beacons_switch\":\"" + beacons_switch + "\",";
    json += "\"igate_beacon_arf\":\"" + igate_beacon_arf + "\",";
    json += "\"beacons_timing\":\"" + beacons_timing + "\",";

    json += "\"meteo_altitude\":\"" + meteo_altitude + "\",";
    json += "\"drift_temperature\":\"" + drift_temperature + "\",";
    json += "\"drift_pressure\":\"" + drift_pressure + "\",";
    json += "\"Zambretti\":\"" + Zambretti + "\",";
    json += "\"Zambretti_enabled\":\"" + Zambretti_enabled + "\",";   // solo per bloccare l'uso del toggle

    json += "\"frequency\":\"" + frequency + "\",";
    json += "\"power\":\"" + power + "\",";
    json += "\"modulation\":\"" + modulation + "\",";

    json += "\"WiFi1_ssid\":\"" + WiFi1_ssid + "\",";
    json += "\"WiFi1_pwd\":\"" + WiFi1_pwd + "\",";
    json += "\"WiFi2_ssid\":\"" + WiFi2_ssid + "\",";
    json += "\"WiFi2_pwd\":\"" + WiFi2_pwd + "\",";
    json += "\"access_point_pwd\":\"" + access_point_pwd + "\",";
    json += "\"access_point_asd\":\"" + access_point_asd + "\",";
    json += "\"aprs_server\":\"" + aprs_server + "\",";
    json += "\"aprs_passcode\":\"" + aprs_passcode + "\",";
    json += "\"aprs_sh\":\"" + aprs_sh + "\",";
    json += "\"wunder_switch\":\"" + wunder_switch + "\",";
    json += "\"wunder_id\":\"" + wunder_id + "\",";
    json += "\"wunder_key\":\"" + wunder_key + "\",";
    
    json += "\"password\":\"" + password + "\",";
    json += "\"gmt_zone\":\"" + gmt_zone + "\",";
    json += "\"drift_battery\":\"" + drift_battery + "\",";
    json += "\"battery_view\":\"" + battery_view + "\",";
    json += "\"telemetry\":\"" + telemetry + "\",";
    json += "\"oled\":\"" + oled + "\",";
    json += "\"INA226_status\":\"" + String(INA226_status ? "ok" : "ko" ) + "\",";
    json += "\"INA226_scale\":\"" + INA226_scale + "\"";
    json += "}";
    
    //Serial.println(json);          //--- mostra il completo file di configurazione
    request->send(200, "application/json", json);
  });


  // Riceve il form e salva i dati
  server.on("/salva", HTTP_POST, [](AsyncWebServerRequest *request) {
  
    if (request->hasParam("igate_call", true)) {
      igate_call = request->getParam("igate_call", true)->value();
    }
    if (request->hasParam("igate_latlong", true)) {
      igate_latlong = request->getParam("igate_latlong", true)->value();
    }
    if (request->hasParam("igate_info", true)) {
      igate_info = request->getParam("igate_info", true)->value();
    }
    if (request->hasParam("igate_backup", true)) {
      igate_backup = request->getParam("igate_backup", true)->value();
    }
    if (request->hasParam("digi_radius", true)) {
      digi_radius = request->getParam("digi_radius", true)->value();
    }
    if (request->hasParam("digi_route", true)) {
      digi_route = request->getParam("digi_route", true)->value();
    }
    if (request->hasParam("digi_denied", true)) {
      digi_denied = request->getParam("digi_denied", true)->value();
    }
    if (request->hasParam("meteo_call", true)) {
      meteo_call = request->getParam("meteo_call", true)->value();
    }
    if (request->hasParam("meteo_latlong", true)) {
      meteo_latlong = request->getParam("meteo_latlong", true)->value();
    }
    if (request->hasParam("meteo_info", true)) {
      meteo_info = request->getParam("meteo_info", true)->value();
    }
    if (request->hasParam("meteo_altitude", true)) {
      meteo_altitude = request->getParam("meteo_altitude", true)->value();
    }
    if (request->hasParam("drift_temperature", true)) {
      drift_temperature = request->getParam("drift_temperature", true)->value();
    }
    if (request->hasParam("drift_pressure", true)) {
      drift_pressure = request->getParam("drift_pressure", true)->value();
    }
    if (request->hasParam("Zambretti", true)) {
      Zambretti = request->getParam("Zambretti", true)->value();
    }
    if (request->hasParam("beacons_switch", true)) {
      beacons_switch = request->getParam("beacons_switch", true)->value();
    }
    if (request->hasParam("beacons_timing", true)) {
      beacons_timing = request->getParam("beacons_timing", true)->value();
    }
    if (request->hasParam("igate_beacon_arf", true)) {
      igate_beacon_arf = request->getParam("igate_beacon_arf", true)->value();
    }

    //===================================================================================
    String old_frequency = frequency;
    if (request->hasParam("frequency", true)) {
      frequency = request->getParam("frequency", true)->value();
    }
    String old_power = power;
    if (request->hasParam("power", true)) {
      power = request->getParam("power", true)->value();
    }
    String old_modulation = modulation;
    if (request->hasParam("modulation", true)) {
      modulation = request->getParam("modulation", true)->value();
    }

    if ( old_frequency != frequency || old_power != power || old_modulation != modulation ) lora_setup();
    //===================================================================================

    if (request->hasParam("WiFi1_ssid", true)) {
      WiFi1_ssid = request->getParam("WiFi1_ssid", true)->value();
    }
    if (request->hasParam("WiFi1_pwd", true)) {
      WiFi1_pwd = request->getParam("WiFi1_pwd", true)->value();
    }
    if (request->hasParam("WiFi2_ssid", true)) {
      WiFi2_ssid = request->getParam("WiFi2_ssid", true)->value();
    }
    if (request->hasParam("WiFi2_pwd", true)) {
      WiFi2_pwd = request->getParam("WiFi2_pwd", true)->value();
    }
    if (request->hasParam("access_point_pwd", true)) {
      access_point_pwd = request->getParam("access_point_pwd", true)->value();
    }
    if (request->hasParam("access_point_asd", true)) {
      access_point_asd = request->getParam("access_point_asd", true)->value();
    }
    if (request->hasParam("aprs_server", true)) {
      aprs_server = request->getParam("aprs_server", true)->value();
    }
    if (request->hasParam("aprs_passcode", true)) {
      aprs_passcode = request->getParam("aprs_passcode", true)->value();
    }
    if (request->hasParam("aprs_sh", true)) {
      aprs_sh = request->getParam("aprs_sh", true)->value();
    }
    if (request->hasParam("wunder_switch", true)) {
      wunder_switch = request->getParam("wunder_switch", true)->value();
    }
    if (request->hasParam("wunder_id", true)) {
      wunder_id = request->getParam("wunder_id", true)->value();
    }
    if (request->hasParam("wunder_key", true)) {
      wunder_key = request->getParam("wunder_key", true)->value();
    }
    if (request->hasParam("password", true)) {
      password = request->getParam("password", true)->value();
    }
    if (request->hasParam("gmt_zone", true)) {
      gmt_zone = request->getParam("gmt_zone", true)->value();
    }
    if (request->hasParam("battery_view", true)) {
      battery_view = request->getParam("battery_view", true)->value();
    }
    if (request->hasParam("drift_battery", true)) {
      drift_battery = request->getParam("drift_battery", true)->value();
    }
    if (request->hasParam("telemetry", true)) {
      telemetry = request->getParam("telemetry", true)->value();
    }
    if (request->hasParam("oled", true)) {
      oled = request->getParam("oled", true)->value();
    }
    if (request->hasParam("INA226_scale", true)) {
      INA226_scale = request->getParam("INA226_scale", true)->value();
    }

    salvaConfig();
    request->redirect("/setup");
  });



  caricaConfig();         //--- caricamento configurazione 'normale'
  verifica_parametri();   //--- verifica configurazione 'normale'
  aprs_latlong();         //--- calcolo sin da subito aprs_latlong 

  WiFi.hostname(igate_call);
  WiFi.mode(WIFI_AP_STA); // Modalità Dual: AP + STA
  if (access_point_pwd == "" ) access_point_pwd = "00000000";
  bool result = WiFi.softAP(igate_call,access_point_pwd);    // Configurazione dell'Access Point
    if (result) {
      AP_active = true;
      Serial.print("Access Point " + igate_call + " ready, IP: ");
      Serial.println(WiFi.softAPIP());
      myip = ipToString(WiFi.softAPIP());
      display.setCursor(0,tmp);
      display.print("setup WiFi");
      tmp=tmp+9;
      display.setCursor(4,tmp);
      display.print("... please wait ...");
      display.display();
    } else Serial.println("Error to start AP");

  if ( WiFi1_ssid != "" ) {
    setup_WiFi( WiFi1_ssid, WiFi1_pwd );
    if ( WiFi.status() != WL_CONNECTED && WiFi2_ssid != "") {
      setup_WiFi( WiFi2_ssid, WiFi2_pwd); 
    }
  }

  NTP_query();          // setta rtc interno
  make_display();
  delay(2000);


  if ( digi_denied == "TEST" ) {
    digi_denied = "";
    salvaConfig();
    igate_mem = "TEST";
    updateFirmware(); // if ( digi_denied == "TEST" ) http.begin(String(OTA_web_server) + "DSCN0177.JPG");
  }

  
  if ( checkForUpdates() == true ) {  //server.end();           // "smonta" l'AsyncWebServer
   updateFirmware();                // quando esegui update  SEMPRE sicuri che ESPAsyncWebServer non sia in funzione
  }

  server.begin();
  lora_setup();
  
  ++bootCount;                                          //Increment boot number and print it every reboot
  Serial.println("Boot number: " + String(bootCount));  // https://randomnerdtutorials.com/esp32-deep-sleep-arduino-ide-wake-up-sources/
  


  lastTlParamBeacon = millis() - int(beacons_timing.toInt()*48 * 60000) + 15000;  // 10 secondi prima delle stringhe parametriche ogni 8 ore
  lastMtBeacon      = millis() - int(beacons_timing.toInt()    * 60000) + 25000;  // 20 secondi prima del primo beacon meteo [ tempo per disattivare lo sleep mode )]
  lastIgBeacon      = millis() - int(beacons_timing.toInt()    * 60000) + 35000;  // beacon igate/digipeater
  lastStMtBeacon    = millis() - int(beacons_timing.toInt()*2  * 60000) + 40000;  // lo status del meteo 2 volte il tempo standard della stringa meteo 
  lastStIgBeacon    = millis() - int(beacons_timing.toInt()*24 * 60000) + 120000; // lo status del igate [ ogni 4 ore ]
  millis_token = millis()-10000;
  timeout_millis = millis();   // valore iniziale per timeout
  cnt_telem = cnt_telemS.toInt();   // ricarica l'ultimo valore del contatore della telemetria 

  Serial.println("\nIW1CGW LoRa system - Rel. " + fw + " ready\n");

}


void loop() {

  //--- LETTURA FLUSSI SULLA SERIALE ------------------------------------
  car = Serial.read();

  if (car == 'x' ) {                        // esce da sleep mode
    while (Serial.read() != '\n') {};
    if ( meteo_mode == "rf_sleep" ) { 
      meteo_mode = "rf";
      Serial.println("sleep mode deactivated");
      salvaConfig();
    }  
  }


  //---------------------------- operazioni cicliche ogni 8 secondi 
  if ( millis() > millis_token +8000 ) {  
    millis_token = millis();
    if (  igate_mem == "reboot" ) ESP.restart();
    token_tx = HIGH;          // abilita trasmissioni LoRa


    //----------------------------------------------- auto reboot dopo 3 giorni = 72 ore = 259200 secondi 
    if ( millis()/1000 > autoreboot ) { 
      OTA_code = 77;
      OTA_logbook();
      display_info("auto","reboot"); 
      ESP.restart();
    } 

    
    //----------------------------------------------- leggi i sensori meteo
    TempC = getTempC();
    Hum = getHum();
    Press = getPressure(TempC);
    DewPoint = getDewPoint();


    //----------------------------------------------- fai scadere il login alla pagina di setup
    if ( millis() - timeout_millis > timeout && tmp_dash_pwd != "" ) {
      Serial.println(".. login scaduto ..");
      tmp_dash_pwd = "";
    }
    

    //----------------------------------------------- switcha le informazioni sul display OLED
    if ( oled == "ok") { 
      oled_switch++;
      if ( oled_switch > 1 ) oled_switch = 0;
      if ( oled_switch == 0 ) make_display();    
      if ( oled_switch == 1 ) if (  BM_sensor_status || AHTstatus ) make_meteo_display();
      display.display();
    }
  

    //----------------------------------------------- aggiorna tensioni batteria / leggi INA226
    tlm_voltage=0;    // tensione 'aux'
    voltage=0;        // batteria del TTGO
    int ptr = 1;
    int tmp_Vint = 0;
    while ( ptr <= 10 ) {                                 // esegue 10 letture del sensore
      tmp_Vint = analogRead( VOLTAGE_SENSOR_PIN );
      delay(10);
      voltage = tmp_Vint + voltage;
      ptr++;
    }

    voltage = voltage/10;                                   // lettura campioni della batteria con circuito di riduzione interna 2:1 rif: GPIO35
    voltage_dashboard = ((3.3/4095)*voltage)*2 + drift_battery.toFloat();   // valore puntuale in Volt batteria TTGO
       
    battPercent = 100 * (voltage_dashboard - 3.3) / (4.13 - 3.3);           // valore puntuale in % carica batteria TTGO
    if (battPercent > 100) battPercent = 100;
    if (battPercent < 0) battPercent = 0;
  
    // --- gestione INA226 ------------------------
    if ( INA226_status ) {     
      ina226.readAndClearFlags();
      shuntVoltage_mV = ina226.getShuntVoltage_mV();
      busVoltage_V = ina226.getBusVoltage_V();
      current_mA = ina226.getCurrent_mA();
      if ( INA226_scale == "10") current_mA = current_mA *10;
      //power_mW = ina226.getBusPower();
      loadVoltage_V  = busVoltage_V + (shuntVoltage_mV / 1000);
      // --- ricalcolo di tlm_voltage perche e installato INA226
      tlm_voltage = int(( loadVoltage_V*255) / voltmeter_param_c);                          //--- parametrizzazione 0-255 per telemetria da 0 a 25.5 Volt
      tlm_current = int(( (current_mA/1000) - amperometer_param_c ) / amperometer_param_b); //--- parametrizzazione 0-255 per telemetria da 0 a 3.200 milliAmpere positivi
      
      //=========================== test
      //loadVoltage_V = 12.3;
      //current_mA = -0.600;
      //Serial.print("voltage: ");Serial.println(loadVoltage_V);
      //Serial.print("current_mA: ");Serial.println(current_mA);
      //=========================== test
       

    }
    // --- gestione INA226 ------------------------


  } //----------------------------- operazioni cicliche ogni 8 secondi 



//-------------------------------------------------- verifica funzionalita server APRS-IS
  if ( aprs_login ) {  
    String aprs_server_rx = (aprsis.readString());
    if ( aprs_server_rx.indexOf("# aprsc") != -1 ) {
      aprs_login_millis = millis();                                       //--- ricevuto banner azzera contatore, OK server APRS-IS per i prossimi 60 secondi
      int GmtIndex = aprs_server_rx.indexOf('T', 1);                   //--- Troviamo la posizione di "GMT" nella stringa
      int SpaceIndex = aprs_server_rx.indexOf(' ', GmtIndex+2);          //--- Troviamo la posizione del primo spazio dopo "GMT"+1 nella stringa
      current_server_aprsis = (aprs_server_rx.substring(GmtIndex+2, SpaceIndex)); //--- ricavo il nome del server APRS-IS cui sono connesso 
    }
    if ( millis() - aprs_login_millis > 60000 ) {                         //--- caduta del network APRS-IS 1 minuto che non ricevo il banner
      Serial.println(".. lost connection to APRS-IS server ..");
      aprs_login = false;                                                 //--- dichiaro la connessioner persa
      if ( igate_backup == "ok" && !backupStatus ) start_bk();                //--- se é previsto il backup ma non é attivo il backup attiva il backup
    }
  }
  //-------------------------------------------------- verifica funzionalita server APRS-IS

  //-------------------------------------------------- verifica caduta connessione WiFi e se del caso attiva il backup
  if ( WiFi.status() != WL_CONNECTED && meteo_mode != "rf_sleep" && WiFi1_ssid != "") {
    if ( igate_backup == "ok" && !backupStatus ) start_bk();                  
    if ( AP_active ) myip = ipToString(WiFi.softAPIP());
    else myip = "0.0.0.0";
    make_display();
    if ( millis() > lost_connection_millis + WiFi_lost_time ) {
      lost_connection_millis = millis();
      WiFi.disconnect();
      delay ( 300 );
      Serial.println(".. i try to reconnect to wifi ..");
      if ( WiFi2_ssid == "" ) WiFi.reconnect();  //--- se non é stata definito il secondo WiFi vai un semplice 'reconnect'
      else {
        setup_WiFi( WiFi1_ssid , WiFi1_pwd ); //--- ritenta sul primo WiFi
        if ( WiFi.status() != WL_CONNECTED ) {
          setup_WiFi( WiFi2_ssid , WiFi2_pwd ); //--- se il primo ha fallito ritenta il secondo 
        }
      }
    }    
  }  
  //-------------------------------------------------- verifica caduta connessione WiFi e se del caso attiva il backup

  //-------------------------------------------------- operazioni a seguito ripristino/presenza  connettivita
  if ( WiFi.status() == WL_CONNECTED ) {                                    
    myip = ipToString(WiFi.localIP());
    if ( !RTC_status ) NTP_query();                 //--- se l'RTC non é settato settalo
    if ( backupStatus ) stop_bk();                  //--- verifica presenza di un backup in stato attivo, esci da backup e ripristina setup
    if ( !aprs_login ) {                                                    //--- se non c'é un login attivo su APRS-IS ma occorre connettiti a APRS-FI
      
      if ( igate == "ok" || ( meteo == "ok" && meteo_mode == "ip" ) ) { // rf / ip / rf_sleep //
        aprsis.stop();
        delay(100);
        Serial.println(".. trying to connect to APRS-IS server ..");
        aprsis_setup();
      } 
    }  
  } 
  //-------------------------------------------------- operazioni a seguito ripristino connettivita


  //----------------------------  auto spegnimento di AP o di tutto il modulo WiFi dopo 'timeout_millis'' se non é configurato un SSiD WiFi
  if ( AP_active && access_point_asd == "ok" && millis() - timeout_millis > timeout ) {
    Serial.println(".. Spengo AP ..");
    WiFi.softAPdisconnect(true);  // spegne l'AP, il client STA resta connesso
    AP_active = LOW;
    delay(350);
    if ( WiFi1_ssid == "" ) {
      Serial.println(".. Spengo WiFi ..");
      WiFi.mode(WIFI_OFF);  // spengo tutto il modulo WiFi
      myip = "no WiFi active";
      make_display();
    }
  }
  //----------------------------  auto spegnimento di AP o di tutto il modulo WiFi dopo 'timeout_millis'' se non é configurato un SSiD WiFi


  //-------------------------------------------------- LoRa RX routine
 
    int packetSize = LoRa.parsePacket();
    if (packetSize) {
      while (LoRa.available()) {
        lora_rx();
      }
    }

  //-------------------------------------------------- LoRa RX routine


  //-------------------------------------------------- svuota buffer WiFiClient aprsis

  while ( igate == "ok" && aprsis.available() || meteo_mode == "ip" && aprsis.available() ) {
    String apstring;
    char aprx = aprsis.read();
    apstring += aprx;
    if (aprx == '\n') {
      if (apstring.indexOf("logresp") == -1) {
      }
      apstring = "";
    }
  }
  //-------------------------------------------------- svuota buffer WiFiClient aprsis


  //-------------------------------------------------- timer dei beacon
  if ( beacons_switch == "ok" ) {
    if ( bootCount == 1 && telemetry == "ok" && millis() - lastTlParamBeacon >= (beacons_timing.toInt()*48 * 60000) ) beacon_telemetry_param();  // beacon stringhe parametriche ogni 8 ore
    if ( millis() - lastMtBeacon  >= (beacons_timing.toInt() * 60000) ) beacon_meteo();   //--- aggiornamento grafici meteo, emissione beacon meteo se abilitato                    
    if ( telemetry != "ok" && millis() - lastStMtBeacon  >= ( beacons_timing.toInt()*2 *  60000 )) beacon_meteo_status(); //--- beacon meteo status ogni 20 minuti se non si usa telemetria per avere nel fumetto i valori di tensione
    if ( telemetry == "ok" && millis() - lastStMtBeacon  >= ( beacons_timing.toInt()*24 * 60000 )) beacon_meteo_status(); //--- beacon meteo status ogni 4 ore se si usa telemetria 
    if ( millis() - lastIgBeacon   >= (beacons_timing.toInt() * 60000) ) beacon_igate();           //--- beacon iGate/telemetria anche su meteo
    if ( millis() - lastStIgBeacon >= (beacons_timing.toInt()*24 * 60000) ) beacon_igate_status(); // lo status ogni 4 ore - se iGate riceve segnali resetta pure il contatore
    if ( millis() - pressureTrendTimeout >= (180 * 60000) && BM_sensor_status ) ZambrettiRoutine();  // ogni 3 ore Zambretti value
  }
  //-------------------------------------------------- timer dei beacon


  //-------------------------------------------------- gestione OLED
  if (oled != "ok"  && mem_oled ) {
    display.dim(true);
    mem_oled = LOW;
  }
  if (oled == "ok"  && !mem_oled ) {
    display.dim(false);
    mem_oled = HIGH;
  }
  //-------------------------------------------------- gestione OLED
}

/*-------------------------------------------------- *** fine di loop ***

RAM:   [==        ]  15.6% (used 51180 bytes from 327680 bytes)
Flash: [========= ]  92.4% (used 1211293 bytes from 1310720 bytes)

*/