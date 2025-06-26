#include "lora_functions.h"
#include "rtc.h"
#include "ota_functions.h"
/*
Chip LoRa	Banda di Frequenza
SX1276	868 MHz / 915 MHz
SX1278	433 MHz
*/



byte pkRx10m; // pacchetti ricevuti  da LoRa nei 10 minuti [ pacchetti validi ]
byte pkTx10m; // pacchetti trasmessi da LoRa nei 10 minuti [ come digipeater o come igate vs APRS-IS ]
int Km;             // Km distanza stazione ascoltata con dati di geo-referenziazione
int dxKm;           // Km DX delle stazioni ascoltate con dati di geo-referenziazione
String LogEvent;    // formazione stringa MH

unsigned long lastDigipeat = 0; /* timer che serve per evitare nei 30 secondi successivi la ritrasmissione 
                                  di uno stesso pacchetto LoRa se é attiva l'opzione di 'ignora nuovo paradigma' */

//========================================

void lora_rx() {
  #include"config.h"
      int pos1, pos2, pos3;
      String destCall, digiPath, sourceCall, message, digiPacket,station_symbol,MIC_E_station_symbol;
      
      String rxPacket = LoRa.readString();
      rxPacket = rxPacket.substring(3);

      //Serial.println("RX: " + rxPacket.substring(0,(rxPacket.length()-1)));
      Serial.println("\nRX: " + rxPacket );

      //--- verifica idoneità pacchetto e rilevamento sourceCall, destCall etc
      pos1 = rxPacket.indexOf('>');
      pos2 = rxPacket.indexOf(':');
      digiPath = "";
      sourceCall = rxPacket.substring(0, pos1);
      message = rxPacket.substring(pos2 + 1);
      destCall = rxPacket.substring(pos1 + 1, pos2); 
      pos3 = destCall.indexOf(',');
      if (pos3 > 0) {
        digiPath = destCall.substring(pos3 + 1);
        destCall = destCall.substring(0, pos3);
      }


      //--- verifica idoneità pacchetto
      byte ignore = 10;                                       //--- valore di default '10'
      if (pos1 < 3) ignore = 254;                             //--- stazioni con errore frame '254
      if (pos2 < pos1) ignore = 254;                          //--- stazioni con errore frame '254
      if (destCall == "") ignore = 254;                       //--- stazioni con errore frame '254

      if ( ignore < 254 && pkRx10m <= 255 ) pkRx10m++;        // incrementa il contatore dei pacchetti ricevuti a 10 minuti
   
      //--------------------------- stazioni fake
        if ( rxPacket.indexOf("IU1CGW") > -1 ) ignore = 253; 
        if ( rxPacket.indexOf("AUTES") > -1 ) ignore = 253; 
        if ( rxPacket.indexOf("N0CALL") > -1 ) ignore = 253; 
        if ( rxPacket.indexOf("MTCGPS") > -1 ) ignore = 253; 
      //--------------------------- stazioni fake

      //--------------------------- stazioni no ip RF only
        if ( rxPacket.indexOf("NOGATE") > -1 ) ignore = 252; 
        if ( rxPacket.indexOf("RFONLY") > -1 ) ignore = 252; 
        if ( rxPacket.indexOf("TCPIP") > -1 ) ignore = 252; 
        if ( rxPacket.indexOf("TCPXX") > -1 ) ignore = 252; 
      //--------------------------- stazioni no ip RF only


      // ------------------------------------------------ effettive funzioni dell' iGate per codici ignore == 10
      if ( ignore == 10 && igate == "ok" ) {
        String igatePacket = rxPacket;
        igatePacket = igatePacket.substring(0, igatePacket.indexOf(":")) + ",qAO," + igate_call + igatePacket.substring(igatePacket.indexOf(":"));
        String Tmp = igatePacket;
        aprsis_send(igatePacket); 
        //OTA_log(Tmp);                                                       //--- solo per me per test !!!
        //if ( pkig10m <= 255 ) pkig10m++;    // pacchetti igate a 10 minuti
        if ( pkTx10m <= 255 ) pkTx10m++;    // pacchetti trasmessi a 10 minuti
      }
      // ------------------------------------------------ effettive funzioni dell' iGate per codici ignore == 10


      //-------------- analisi simboli stazioni ascoltate

      station_symbol = rxPacket.substring(pos2+10,pos2+11) + rxPacket.substring(pos2+20,pos2+21);
      MIC_E_station_symbol = rxPacket.substring(pos2+2, pos2+3 ) + rxPacket.substring(pos2+11,pos2+12);

      //--- tabella dei simboli ammessi per il digipeating
      if ( station_symbol == "La" || MIC_E_station_symbol == "La" ) ignore = 251; // LoRa igate icona rossa ( ! gulp ! )
      if ( station_symbol == "L_" || MIC_E_station_symbol == "L_" ) ignore = 251; // LoRa pseudometeo icona bollo blu con simbolo L ( ! gulp ! )
              
      if ( station_symbol == "L#" || MIC_E_station_symbol == "L#" ) ignore = 2; // LoRa DIGI
      if ( station_symbol == "L&" || MIC_E_station_symbol == "L&" ) ignore = 2; // LoRa igate icona nera
      if ( station_symbol == "I#" || MIC_E_station_symbol == "I#" ) ignore = 2; // I-gate equipped digipeater
      if ( station_symbol == "R[" || MIC_E_station_symbol == "R[" ) ignore = 2; // Runner
      if ( station_symbol == "H[" || MIC_E_station_symbol == "H[" ) ignore = 2; // Hiker
      if ( station_symbol == "/[" || MIC_E_station_symbol == "/[" ) ignore = 2; // Human/Person
      if ( station_symbol == "/a" || MIC_E_station_symbol == "/a" ) ignore = 2; // AMBULANCE  
      if ( station_symbol == "/<" || MIC_E_station_symbol == "/<" ) ignore = 2; // Motorcycle 
      if ( station_symbol == "/>" || MIC_E_station_symbol == "/>" ) ignore = 2; // CAR
      if ( station_symbol == "/F" || MIC_E_station_symbol == "/F" ) ignore = 2; // Farm Vehicle (tractor)
      if ( station_symbol == "/O" || MIC_E_station_symbol == "/O" ) ignore = 2; // BALLOON
      if ( station_symbol == "/U" || MIC_E_station_symbol == "/U" ) ignore = 2; // BUS
      if ( station_symbol == "/_" || MIC_E_station_symbol == "/_" ) ignore = 2; // WEATHER Station
      if ( station_symbol == "/b" || MIC_E_station_symbol == "/b" ) ignore = 2; // BIKE
      if ( station_symbol == "/j" || MIC_E_station_symbol == "/j" ) ignore = 2; // JEEP
      if ( station_symbol == "/k" || MIC_E_station_symbol == "/k" ) ignore = 2; // TRUCK
      if ( station_symbol == "/p" || MIC_E_station_symbol == "/p" ) ignore = 2; // ROVER (puppy, or dog)
      if ( station_symbol == "/u" || MIC_E_station_symbol == "/u" ) ignore = 2; // TRUCK (18 wheeler)           
      if ( station_symbol == "/v" || MIC_E_station_symbol == "/v" ) ignore = 2; // VAN   
      if ( station_symbol == "/C" || MIC_E_station_symbol == "/C" ) ignore = 2; // kayak “/C” 
                  
      //--- tabella dei simboli ammessi per il digipeating

      

      //--- calcolo distanza da sourceCall [ solo ascolto diretto e pacchetti con georeferenziazione :! ]   
      Km = 0;
      if ( rxPacket.indexOf("*") == -1 && sourceCall != "" &&  rxPacket.indexOf(":!") > -1 || rxPacket.indexOf("*") == -1 && sourceCall != "" &&  rxPacket.indexOf(":=") > -1 ) {    
        
        LogEvent = rtc.getTime();
        calc_dist(rxPacket,pos2); 
        if ( Km > digi_radius.toInt()  ) {                // from string to int
            ignore = 248;                                  //--- stazione con digipeating negato per radius eccessivo
        }

        //--- caricamento MHeard -------------------------------------------------- 
        int shift = 41;
        do { 
          shift--;
          //Serial.println(shift);
          MHeard[shift+1] = MHeard[shift]; 
        } while ( shift >= 1);
              
        MHeard[0] = LogEvent + " " + String(sourceCall) + " SNR " + String(int(LoRa.packetSnr())) + "dB RSSI " + String(LoRa.packetRssi()) + "dBm";
        if ( Km > 0 ) MHeard[0] += " " + String(Km) + "Km";
        lastRX = MHeard[0];   // l'ultima ricezione corrisponde al valore '0' della matrice
        if ( Km >= dxKm ) {    // se questa ultima ricezione ha Km superiori all'ultimo segnato come dxKm 
          dxKm = Km;          // segna che i nuovi Km DX sono proprio questi ultimi
          dxRX = MHeard[0];   // segna che la nuova stazione DX é proprio quest'ultima

        }
        //--- caricamento MHeard -------------------------------------------------- 
       
      } 

      
      // --------------------- regola per stazione che ha un digipeating esclusivo 
      if ( digi_route != "" && sourceCall == digi_route ) ignore = 1;
            
      
      // --------------------- regola per consentire digipeating telemetrie e messaggi 
      //if ( rxPacket.indexOf(":T#") != -1  || rxPacket.indexOf(":EQNS.") != -1 || rxPacket.indexOf("UNIT.") != -1 || rxPacket.indexOf("PARM.") != -1 || rxPacket.indexOf("BITS.") != -1 ) ignore = 3;
      // ---------- se metti i : passa l'universo, volpe ! 
      //if ( rxPacket.indexOf(":") != -1 || rxPacket.indexOf("?") != -1 || rxPacket.indexOf("<") != -1 || rxPacket.indexOf("T#") != -1 ) ignore = 3;
       if ( rxPacket.indexOf("?") != -1 || rxPacket.indexOf("<") != -1 || rxPacket.indexOf("T#") != -1 || rxPacket.indexOf("EQNS") != -1 || rxPacket.indexOf("UNIT") != -1 || rxPacket.indexOf("PARM") != -1 || rxPacket.indexOf("BITS") != -1 ) ignore = 3;
       

      // --------------------- regola per stazione che ha un digipeating negato 
      if ( digi_denied.equals(sourceCall) ) ignore = 249;   


      // --------------------- regola per stazione già ripetuta una volta  
      if ( rxPacket.indexOf("*") > 1 ) ignore = 250; 

      if ( debug_digi ) { 
          Serial.print(".. ignore code: ");  Serial.print(ignore);  Serial.print(" ");
          if ( ignore == 1 ) Serial.println("** OK station in exclusive digipeater route");  
          if ( ignore == 2 ) Serial.println("** OK station symbol ok in digipeater route"); 
          if ( ignore == 3 ) Serial.println("** OK telemetrix & msg in digipeater route");  
          if ( ignore == 4 ) Serial.println("** OK query message");  
          //================================================================
          if ( ignore == 10 ) Serial.println("** data not managed in digipeating"); 
          if ( ignore == 254 ) Serial.println("** bad packet 'destcall' not found");
          if ( ignore == 253 ) Serial.println("** station fake");
          if ( ignore == 252 ) Serial.println("** station no via ip only RF but not digipeat"); 
          if ( ignore == 251 ) Serial.println("** bad packet not compliance aprs.org");
          if ( ignore == 250 ) Serial.println("** station already repeated");
          if ( ignore == 249 ) Serial.println("** station banned for digipeating"); 
          if ( ignore == 248 ) { 
            Serial.print("** station @ ");Serial.print(Km);
            Serial.println( "Km, over max digipeater radius @ ");
          } 
      }
  
      // -------------------------------------------- effettivo digipeater per i soli codici ignore minori di 10
      if ( digi == "ok"  && ignore < 10 ) {
          if (int callIndex = digiPath.indexOf(igate_call) > -1 && digiPath.indexOf(igate_call + "*") == -1) {
            digiPath = digiPath.substring(0, callIndex - 1) + digiPath.substring(callIndex + igate_call.length());
          }
          if (int paradigmIndex = digiPath.indexOf("WIDE1-") > -1 && digi == "ok" && digiPath.indexOf(igate_call + "*") == -1 && rxPacket.indexOf(meteo_call) == -1 && sourceCall.indexOf(igate_call) == -1) {
            paradigmIndex = digiPath.indexOf("WIDE1-");
            if (paradigmIndex == 0) paradigmIndex = 1;
            if (paradigmIndex == 1) digiPath = digiPath.substring(0, paradigmIndex - 1) + "," + igate_call + "*,WIDE1*" + digiPath.substring(paradigmIndex + 6);
            else digiPath = digiPath.substring(0, paradigmIndex - 1) + "," + igate_call + "*,WIDE1*" + digiPath.substring(paradigmIndex + 7);
            if (digiPath.indexOf(",") != 0) digiPath = "," + digiPath;
            digiPacket = sourceCall + ">" + destCall + digiPath + ":" + message;
            delay(750);             //--- 20/02/2025
            lora_send(digiPacket);
            if ( pkTx10m <= 255 ) pkTx10m++;    // pacchetti trasmessi a 10 minuti

          } else if ( digi == "ok" && DIGI_IGNORE_PARADIGM && digiPath.indexOf("*") == -1 && (millis() > lastDigipeat + 30000 || lastDigipeat == 0) && digiPath.indexOf(igate_call + "*") == -1 && rxPacket.indexOf(meteo_call) == -1 && sourceCall.indexOf(igate_call) == -1) {
            //else if (digiSwitch && DIGI_IGNORE_PARADIGM && digiPath.indexOf("*") == -1 && (millis() > lastDigipeat + 600000 || lastDigipeat == 0) && digiPath.indexOf(String(IGATE_CALLSIGN) + "*") == -1 && rxPacket.indexOf(String(METEO_CALLSIGN)) == -1 && sourceCall.indexOf(String(IGATE_CALLSIGN)) == -1) {
            //else if (digiSwitch && DIGI_IGNORE_PARADIGM && digiPath.indexOf("*") == -1 && (millis() > lastDigipeat + 600000 || lastDigipeat == 0)                                                                                                                                                              ) {
            
            lastDigipeat = millis();
            digiPath = digiPath + "," + igate_call + "*";
            if (digiPath.indexOf(",") != 0) {
              digiPath = "," + digiPath;
              Serial.print("digiPath.indexOf(',') + '*' digiPath: ");Serial.println(digiPath);
            }
            digiPacket = sourceCall + ">" + destCall + digiPath + ":" + message;
            delay(750);             //--- 20/02/2025
            lora_send(digiPacket);
            if ( pkTx10m <= 255 ) pkTx10m++;    // pacchetti digipeater a 10 minuti
          } else if (digi == "ok" && DIGI_IGNORE_PARADIGM) {
              Serial.println("Station not repeated.");
            }
        } //--- end modulo digipeater 
      // -------------------------------------------- effettivo digipeater per i soli codici ignore minori di 10
     
      if ( USE_LASTRX_STATUS &&  ignore <= 10 && sourceCall != "" && sourceCall != igate_call && sourceCall != meteo_call && beacons_switch == "ok" && igate == "ok" ) {
        String statusMessage = igate_call + ">APLHI0:>Last RX: " + sourceCall + " SNR=" + String(LoRa.packetSnr()) + "dB RSSI=" + String(LoRa.packetRssi()) + "dBm";
        if ( Km > 0 ) statusMessage = statusMessage + " - " + String(Km) + "Km";
         aprsis_send(statusMessage);
        lastStIgBeacon=millis();    // per beacons_timing.toInt()*24 non verrà inviato lo status 'istituzionale' cosi rimane l'ultima stazione ricevuta.
      } 
   
  } // --- end pacchetto valido con preambolo: \x3c\xff\x01


void lora_send(String tx_data) {
  #include"config.h"
  digitalWrite(PLed_life_int, HIGH);
  //Serial.println("TX: " + tx_data.substring(0,(tx_data.length()-1)));
  Serial.println("TX: " + tx_data);

  LoRa.beginPacket();
  LoRa.write('<');
  LoRa.write(0xFF);
  LoRa.write(0x01);
  
  LoRa.write((const uint8_t *)tx_data.c_str(), tx_data.length());
  LoRa.endPacket();

  millis_token = millis();
  token_tx = false;
  
  digitalWrite(PLed_life_int, LOW);
}


void lora_setup() {
  
  salvaConfig();
  SPI.begin();
  delay(300);
                                                  //   *** spostato in setup della main.cpp
  SPI.begin(LoRa_SCK, LoRa_MISO, LoRa_MOSI, LoRa_SS);
  LoRa.setPins(LoRa_SS, LoRa_RST, LoRa_DIO0);

  int s = frequency.indexOf('.');
  if ( s <= 0 ) {
    frequency = "433.775";
    return;
  }
  
  String MHz = frequency.substring(s-3, s);
  String KHz = frequency.substring(s+1, s+4);
  frequency = MHz + "." + KHz;
  long freq = int(MHz.toInt()*1000000 + KHz.toInt()*1000);

  if (!LoRa.begin(freq)) {
      Serial.println("Failed to setup LoRa module.");
      while (1);
    }
  
  delay(350);

  if ( modulation == "300" ) {  
    LoRa.setSignalBandwidth(LoRa_SignalBandwidth);
    LoRa.setSpreadingFactor(LoRa_SpreadingFactor);    // --- parametri std
    LoRa.setCodingRate4(LoRa_CodingRate4);
  }

  if ( modulation == "1200") {  
    LoRa.setSignalBandwidth(LoRa_SignalBandwidth);
    LoRa.setSpreadingFactor(LoRa_XSpreadingFactor_poland);    // --- parametri poland
    LoRa.setCodingRate4(LoRa_XCodingRate4_poland);
  }

  delay(350);
 
  
  //Serial.print(F("frequency: "));Serial.println(freq);
  //LoRa.setGain(LoRa_rx_gain);
  // delay(1000);
  

  LoRa.enableCrc();
  delay(350);

  Serial.print("LoRa setup: ok - freq: ");Serial.println(freq);
}


