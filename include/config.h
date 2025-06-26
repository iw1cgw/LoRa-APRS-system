// GENERAL SETTINGS

#define Release "1.00_250626"
#define Hostname "LoRa_APRS_system"         // Hostname, name of computer visible in the network

#define DEFAULT_STATUS "https://iw1cgw.wordpress.com/lora/" 


#define OTA_dirName "/"    
#define OTA_fileName_logbook ""    
#define OTA_fileName_ver ""       
#define OTA_fileName_update ""     



#define OTA_web_server ""    

//
//------------------------------------ digi/igate - parametri ridotti senza IN226
//
#define telem0_param0 ":EQNS.0,1,0,0,1,0,0,0.0196,0"   
#define telem0_param1 ":UNIT.Pkt/10m,Pkt/10m,Volt"
#define telem0_param2 ":PARM.RxPkt,TxPkt,Batt"    
//
//------------------------------------ digi/igate - parametri con IN226
//
#define telem1_param0 ":EQNS.0,1,0,0,1,0,0,0.0196,0,0,0.1,0,0,0.01,-1.27"   
#define telem1_param1 ":UNIT.Pkt/10m,Pkt/10m,Volt,Volt,Ampere"
#define telem1_param2 ":PARM.RxPkt,TxPkt,Batt,Volt Aux,Ampere Aux"       // telemetria con INA226 montato
//
//------------------------------------ solo METEO - parametri ridotti senza IN226
//
#define telem2_param0 ":EQNS.0,0.0196,0"  
#define telem2_param1 ":UNIT.Volt"
#define telem2_param2 ":PARM.Batt"       // telemetria ridotto sola batteria TTGO
//
//------------------------------------ solo METEO - parametri con IN226
//
#define telem3_param0 ":EQNS.0,0.0196,0,0,0.1,0,0,0.01,-1.27"   
#define telem3_param1 ":UNIT.Volt,Volt,Ampere"
#define telem3_param2 ":Batt,Volt Aux,Ampere Aux"       // telemetria con INA226 montato
//
//------------------------------------ parametro comune
//
#define telem_param3 ":BITS.00000000,Telemetry by IW1CGW"



#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */


#define autoreboot 259200   // 72 ore - valore in secondi
#define timeout 300000       // 5 minuti - valore in millisecondi - timeout per spegnimento AP e scadenza login

#define WiFi_lost_time 120000                // 120 secondi - valore in millisecondi

#define VOLTAGE_SENSOR_PIN 35       // use only ADC_5 pins

#define amperometer_param_b 0.01
#define amperometer_param_c -1.27
#define voltmeter_param_b 0.1
#define voltmeter_param_c 25.5

#define PLed_life_int 25            // pin number for LED - 25 is internal led green
#define PLed_life 12                // pin number for LED [ ok 12  ]

#define APRS_IS_Port 14580                  // server port, keep default (14580)

#define USE_LASTRX_STATUS true             // display Last RX status on igate

#define SERIAL_BAUD 115200                 // serial baud

#define LoRa_SCK 5
#define LoRa_MISO 19
#define LoRa_MOSI 27
#define LoRa_SS 18
#define LoRa_RST 14
#define LoRa_DIO0 26

//-------------------------------------------// parametri OESTYLE standard - standard 300 baud
#define LoRa_SpreadingFactor 12         // The Spreading Factor varies between 128 = 2^7 (SF7) and 4096 = 2^12 (SF12)
#define LoRa_SignalBandwidth 125000
#define LoRa_CodingRate4 5
#define LoRa_type "BW 125KHz CR 4:5 SF 12 (300bps)"
/*
set 5 for 4:5
set 6 for 4:6
set 7 for 4:7
set 8 for 4:8
*/

//-------------------------------------------// parametri polacchi, ma vedi anche; https://meshtastic.org/docs/overview/radio-settings
#define LoRa_XSpreadingFactor_poland 9           
#define LoRa_XCodingRate4_poland 7
#define LoRa_type_poland "BW 125KHz CR 4:7 SF 9 (1200bps)"


// DEBUG SETTINGS
#define INA226_DEBUG false                // enable debug sensor INA226
#define DIGI_IGNORE_PARADIGM false         // digipeat packets regardless if they contain WIDEn-N
#define debug_digi false