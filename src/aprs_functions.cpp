#include "APRS_functions.h"

String generateEncodedTelemetry();
String generateEncodedTelemetryBytes(int tempValue);
String encodedBytes;
String telemetry_data;                  //--- costruzione della stringa di telemetria

char igate_lat_APRS[10];
char igate_lon_APRS[10];
char meteo_lat_APRS[10];
char meteo_lon_APRS[10];

String igate_lat;
String igate_lon;

byte cnt_param;
#define DESTCALL_digi    "APLHI0"      
#define DESTCALL_meteo   "APLHM0"

//-------------------------------------------------------------------

void aprsis_setup() {   
  #include"config.h"
  aprsis.stop();
  delay(1000);
  aprs_login = LOW;
  String Tmp;
  char tmp_buffer[40] = ""; 
  aprs_server.toCharArray(tmp_buffer,50);           // da string a char
  if (aprsis.connect(tmp_buffer,APRS_IS_Port )) {
    Serial.println("server APRS-IS connected");
    delay(5000);
  } else {
    Serial.println("server APRS-IS error");
    return;
  }
  
  Tmp = igate_call; 
    if ( aprs_sh == "ok" ) {
    int p = igate_call.indexOf('-', 1);                   //--- Troviamo la posizione di "-" nella stringa
    Tmp = Tmp.substring(0, p) +"-S";                      //--- modifico igate_call
  }
    
  if ( aprs_sh !="ok" && igate !="ok" && meteo_mode == "ip" ) Tmp = meteo_call;
   
  if ( igate == "ok" || meteo_mode == "ip" ) {
    aprsis.println("user " + Tmp + " pass " + aprs_passcode + " vers IW1CGW_LoRa_system " + String(Release) + " filter m/5");
  }
  
  int retr = 0;
  Tmp = "";
    while ( aprsis.available() || retr > 40 ) {
      Tmp = (aprsis.readString());
      Serial.print(Tmp);
      if ( Tmp.indexOf("# aprsc") != -1 ) {
        if ( igate_backup == "ok" && backupStatus ) stop_bk();
        aprs_login = true;
        aprs_login_millis = millis();
      }
      delay(350);
      retr++;
    }

}


void aprsis_send(String aprsis_packet) {
  if (WiFi.status() == WL_CONNECTED && aprs_login) {
    aprsis_packet.trim();
    Serial.println("APRS-IS: " + aprsis_packet);
    aprsis.println(aprsis_packet + "\r\n");
    //delay(300);
  } else {
    Serial.println(F(".. WARNING - WiFi or APRS-IS server not available .."));
  }
}


void aprs_latlong() {

//-----------------------------------------------// separazione latitudine / longitudine per igate
  int s = igate_latlong.indexOf(',');
  igate_lat = igate_latlong.substring(0, s);
  igate_lat.trim();
  igate_lon = igate_latlong.substring(s+1, igate_latlong.length());
  igate_lon.trim();
  igate_latlong = igate_lat+","+igate_lon;

  //-----------------------------------------------// separazione latitudine / longitudine per meteo
  meteo_latlong.indexOf(',');
  String meteo_lat = meteo_latlong.substring(0, s);
  meteo_lat.trim();
  String meteo_lon = meteo_latlong.substring(s+1, meteo_latlong.length());
  meteo_lon.trim();
  meteo_latlong = meteo_lat+","+meteo_lon;

  //-----------------------------------------------// conversione latitudine per APRS - igate
  char igate_lat_C[10];
  igate_lat.toCharArray(igate_lat_C, 10); // converto stringa in char
  String sign = "N";
    if (igate_lat_C[0] == '-' ) {
      sign = "S";
      igate_lat_C[0] = ' ';
    }
  int gradi=atoi(igate_lat_C);
  double decimali = (atof(igate_lat_C)-gradi)*60;
  sprintf(igate_lat_APRS, "%02d%05.2f%s",gradi,decimali,sign.c_str());
  
  //-----------------------------------------------// conversione longitudine per APRS - igate
  char igate_lon_C[10]; igate_lon.toCharArray(igate_lon_C, 10); // converto stringa in char
  sign = "E";
  if (igate_lon_C[0] == '-' ) {
    sign = "W";
    igate_lon_C[0] = ' ';
  }  
  gradi=atoi(igate_lon_C);
  decimali = (atof(igate_lon_C)-gradi)*60;
  sprintf(igate_lon_APRS, "%03d%05.2f%s",gradi,decimali,sign.c_str()); 
  
  //-----------------------------------------------// conversione latitudine per APRS - meteo
  
  char meteo_lat_C[10];
  meteo_lat.toCharArray(meteo_lat_C, 10); // converto stringa in char
  sign = "N";
    if (meteo_lat_C[0] == '-' ) {
      sign = "S";
      meteo_lat_C[0] = ' ';
    }
  gradi=atoi(meteo_lat_C);
  decimali = (atof(meteo_lat_C)-gradi)*60;
  sprintf(meteo_lat_APRS, "%02d%05.2f%s",gradi,decimali,sign.c_str());
  
  //-----------------------------------------------// conversione longitudine per APRS - meteo
  char meteo_lon_C[10];
  meteo_lon.toCharArray(meteo_lon_C, 10); // converto stringa in char
  sign = "E";
  if (meteo_lon_C[0] == '-' ) {
    sign = "W";
    igate_lon_C[0] = ' ';
  }  
  gradi=atoi(meteo_lon_C);
  decimali = (atof(meteo_lon_C)-gradi)*60;
  sprintf(meteo_lon_APRS, "%03d%05.2f%s",gradi,decimali,sign.c_str()); 
  
  //-----------------------------------------------
  /*
  Serial.print("igate_lat_APRS: "); Serial.println(igate_lat_APRS);
  Serial.print("igate_lon_APRS: "); Serial.println(igate_lon_APRS);
 
  Serial.print("meteo_lat_APRS: "); Serial.println(meteo_lat_APRS);
  Serial.print("meteo_lon_APRS: "); Serial.println(meteo_lon_APRS);
  */

}


void calc_dist(String rxPacket,int pos2) {
  //IU1FIL-14>APLRT1,WIDE1-1,qAO,IR1CH-11                    :!/81)+PV-kk<&QQRX 145.500 Bat=V (mA)
  //IW1CGW-13>APHRM0,WIDE2-2,qAO,IZ1VCX-10                   :!4419.22N/00748.58E_.../...g...t055r...p...P...h89b10104 IW1CGW weather v.12 [1:0]
  //String rxPacket = "IU1FIL-14>APLRT1,WIDE1-1,qAO,IR1CH-11 :!/81)+PV-kk<&QQRX 145.500 Bat=V (mA)";
  //String rxPacket = "IW1CGW-13>APHRM0,WIDE2-2,qAO,IZ1VCX-10:!4419.22N/00748.58E_.../...g...t055r...p...P...h89b10104 IW1CGW weather v.12 [1:0]";
  //IQ2SW-10>APLRG1,TCPIP*,qAC,T2CHILE                       :!L7H1XPh+#a xGLoRa APRS iGATE-DIGI ARI-Saronno
  //IK2XRO-10>APLRG1,IU2SKJ-10*,qAO,IQ4FE-2                  :!L7ScFPp"pa xGLoRa APRS Batt=4.29V
  //IW1QAF-23>APLRW1,WIDE1-1,qAR,IZ1HKE-10                   :!/8%k8PZxo_ xG252/013g021t061r000p002L000h94b09580Experimental LoRa APRS Wx Station
  //IU1FIL-14>APLRT1,WIDE1-1,qAO,IU1LCU-11                   :!/8/gKPW22k@\Q Bat=V (mA)

    float myLat_igate;
    float myLon_igate;
    float LatRx; 
    float LonRx; 

  if ( rxPacket.substring(pos2 +6, pos2+7) == "." && rxPacket.substring(pos2 +16, pos2+17) == "." ) { 
    float LatRx_g = atoi((rxPacket.substring(pos2 +2, pos2+4)).c_str());
    float LatRx_m = atoi((rxPacket.substring(pos2 +4, pos2+6)).c_str());
    float LatRx_s = atoi((rxPacket.substring(pos2 +7, pos2+9)).c_str());
    String LatSign = rxPacket.substring(pos2 +9, pos2+10);
    //Serial.print("LatSign: ");Serial.println(LatSign)

    float LonRx_g = atoi((rxPacket.substring(pos2 +12, pos2+14)).c_str());
    float LonRx_m = atoi((rxPacket.substring(pos2 +14, pos2+16)).c_str());
    float LonRx_s = atoi((rxPacket.substring(pos2 +17, pos2+19)).c_str());
    String LonSign = rxPacket.substring(pos2 +19, pos2+20);
    //Serial.print("LonSign: ");Serial.println(LonSign);

    LatRx = LatRx_g + (LatRx_m/60) + (LatRx_s/6000);
    LonRx = LonRx_g + (LonRx_m/60) + (LonRx_s/6000);
    if ( LatSign == "S" ) LatRx = LatRx * -1;
    if ( LonSign == "W" ) LonRx = LonRx * -1;
  }

  else {
        //const String& GPSPacket         = rxPacket.substring(rxPacket.indexOf(":")+3);
        const String& GPSPacket         = rxPacket.substring(pos2+3);
        
        const String& encodedLatitude   = GPSPacket.substring(0,4);
        const String& encodedLongtitude = GPSPacket.substring(4,8);
        int Y1 = int(encodedLatitude[0]);
        int Y2 = int(encodedLatitude[1]);
        int Y3 = int(encodedLatitude[2]);
        int Y4 = int(encodedLatitude[3]);
        LatRx = 90.0 - ((((Y1-33) * pow(91,3)) + ((Y2-33) * pow(91,2)) + ((Y3-33) * 91) + Y4-33) / 380926.0);
        
        int X1 = int(encodedLongtitude[0]);
        int X2 = int(encodedLongtitude[1]);
        int X3 = int(encodedLongtitude[2]);
        int X4 = int(encodedLongtitude[3]);
        LonRx = -180.0 + ((((X1-33) * pow(91,3)) + ((X2-33) * pow(91,2)) + ((X3-33) * 91) + X4-33) / 190463.0);
  }

//igate_latlong = igate_lat+","+igate_lon;
//const long  gmtOffset_sec = int(gmt_zone.toInt()*3600); // pippo

    /*
    myLat_igate = atof(lat_igate);
    myLon_igate = atof(lon_igate);
    */

    myLat_igate = igate_lat.toFloat();
    myLon_igate = igate_lon.toFloat();

  Km = calculateDistance(myLat_igate, myLon_igate, LatRx, LonRx)/1000;

  if ( LatRx == 0 || LonRx == 0 ) Km = 0;

  /*
  Serial.print("LatRx: ");Serial.println(LatRx);
  Serial.print("LonRx: ");Serial.println(LonRx); 
  Serial.print("RX: ");Serial.print(LatRx,8);Serial.print(",");Serial.print(LonRx,8);
  Serial.print(" - da km: ");Serial.println(Km);
  */
}


String generateEncodedTelemetryBytes(int tempValue) { 
  //String encodedBytes;
  //int tempValue;

  int firstByte   = tempValue / 91;
  tempValue       -= firstByte * 91;

  encodedBytes    = char(firstByte + 33);
  encodedBytes    += char(tempValue + 33);
  //Serial.print("encodedBytes: ");Serial.println(encodedBytes);
  return encodedBytes;
}


String generateEncodedTelemetry() {         // genera la stringa di telemetria in formato compresso
  
  
  // https://github.com/hessu/aprs-specs/blob/master/aprs-base91-comment-telemetry.txt


  telemetry_data = "|";
  generateEncodedTelemetryBytes(cnt_telem); //--- progressivo contatore
  telemetry_data += encodedBytes;
  
  generateEncodedTelemetryBytes(pkRx10m); //--- pacchetti ricevuti nei 10 minuti
  telemetry_data += encodedBytes;
  generateEncodedTelemetryBytes(pkTx10m); //--- pacchetti trasmessi nei 10 minuti
  telemetry_data += encodedBytes;
  generateEncodedTelemetryBytes(int(voltage_dashboard/0.0196)); //--- volt TTGO
  telemetry_data += encodedBytes;

  if ( INA226_status ) {
    generateEncodedTelemetryBytes(tlm_voltage);                   //--- volt esterni
    telemetry_data += encodedBytes;
    generateEncodedTelemetryBytes(tlm_current);                   //--- corrente INA266
    telemetry_data += encodedBytes;
  }

  telemetry_data += "|";

  pkRx10m=0;    // azzera contatore pacchetti
  pkTx10m=0;    // azzera contatore pacchetti

  if ( cnt_telem == 255 ) cnt_telem=0;
  else cnt_telem++;

  salvaConfig();    // salva valore corrente di cnt_telemS
  
  
  //                                il vecchio programma salvava il valore progressivo dei pacchetti
  //EEPROM.write(350,cnt_telem);
  //EEPROM.commit();
  //delay(30);

  return telemetry_data; //--- restituisce stringa telemetria da appiccicare al fondo
}


String getTempAPRS() {
  
  if (BM_sensor_status ) {
  float fahrenheit = TempC;
  fahrenheit *= 9;
  fahrenheit /= 5;
  int ifahrenheit = fahrenheit + 32;
  
  int ifahrenheit_abs = abs(ifahrenheit);
  String sfahrenheit = String(ifahrenheit_abs);
  if ( ifahrenheit_abs < 100 && ifahrenheit >= 0 ) sfahrenheit = String("0") + sfahrenheit;
  if ( ifahrenheit_abs < 10                      ) sfahrenheit = String("0") + sfahrenheit;
  if ( ifahrenheit < 0 ) sfahrenheit = String("-") + sfahrenheit;
  return sfahrenheit;
  } else return "...";
}


String getHumAPRS() {
  if (AHTstatus || BMEstatus ) {
    String sHum = String(int(Hum));
    return sHum;
  } else {
    return "..";
  }
}


String getPressureAPRS() {
  if (BM_sensor_status) {
    //float tmp_press = getPressure();
    int press = (Press*10);
    if (press > 99999) press = 0;
    String spress = String(press);
    if (press < 10000) spress = String("0") + String(spress);
    if (press < 1000) spress = String("0") + String(spress);
    if (press < 100) spress = String("0") + String(spress);
    if (press < 10) spress = String("0") + String(spress);
    return spress;
  } else {
    return ".....";
  }
}


void beacon_igate() {     // invia anche dati telemetria
   
  if ( token_tx ) {
    #include"config.h"
    char tmp_buffer[60] = ""; 
    aprs_latlong();           // calcola lat long per APRS
    
    if ( telemetry == "ok" ) generateEncodedTelemetry();    // gestisce anche numerazione progressiva del contatore e salvataggio del medesimo
   
    if ( igate == "ok" ) {
      String Tmp = igate_call + ">" + String(DESTCALL_digi)             + ":!" + String(igate_lat_APRS) + "L" + String(igate_lon_APRS) + "&" + String(igate_info);
      if (telemetry != "ok" && battery_view == "ok" ) Tmp = Tmp + " | TTGO batt:" + String(voltage_dashboard)+"V";
      //if (telemetry != "ok" && tlm_aux == "ok" ) Tmp = Tmp + " | aux:" + String(tlm_voltage*voltmeter_param_b)+"V";
      if (telemetry == "ok" ) Tmp += telemetry_data;
      //Serial.println(Tmp);
      aprsis_send (Tmp);
    }

    if ( digi == "ok" || igate_beacon_arf == "ok" && igate == "ok" ) {
      String Tmp_symbol = "#";
      if ( igate == "ok" && igate_beacon_arf == "ok" ) Tmp_symbol = "&";
      String Tmp = igate_call + ">" + String(DESTCALL_digi) + ",WIDE1-1" + ":!" + String(igate_lat_APRS) + "L" + String(igate_lon_APRS) + Tmp_symbol + String(igate_info);
      if (telemetry != "ok" && battery_view == "ok" ) Tmp = Tmp + " | TTGO batt:" + String(voltage_dashboard)+"V";
      //if (telemetry != "ok" && tlm_aux == "ok" ) Tmp = Tmp + " | aux:" + String(tlm_voltage*voltmeter_param_b)+"V";
      if (telemetry == "ok" ) Tmp += telemetry_data;
      lora_send (Tmp);
    }

    if ( digi != "ok" && igate != "ok" && telemetry == "ok" ) {   //telemetria su emissioni di solo meteo
       String Tmp = ",WIDE1-1";
      if ( meteo_mode == "ip" ) Tmp = "";
      if ( INA226_status ) {
        sprintf(tmp_buffer, "%s>%s%s:T#%03d,%03d,%03d,%03d",(meteo_call.c_str()), (DESTCALL_meteo),(Tmp.c_str()),(cnt_telem),( int(voltage_dashboard/0.0196) ),(tlm_voltage),(tlm_current));    
      } else {
        sprintf(tmp_buffer, "%s>%s%s:T#%03d,%03d",(meteo_call.c_str()), (DESTCALL_meteo),(Tmp.c_str()),(cnt_telem),( int(voltage_dashboard/0.0196) ));    
      }

      if ( meteo_mode == "rf" || meteo_mode == "rf_sleep") {

        lora_send (String(tmp_buffer));
        if ( meteo_mode == "rf_sleep" ) start_sleep();  //--- poni in sleep mode
      }
      
      if ( meteo_mode == "ip" ) {
        aprsis_send (String(tmp_buffer));
        //Serial.println(String(tmp_buffer));
      }
    }
    lastIgBeacon=millis();   // azzera contatore dell'invio solo dopo aver inviato la stringa
  }  
}


void beacon_meteo() {

  if ( token_tx ) {
  
    /*
    if (BM_sensor_status || AHTstatus) {
      String stemp = String(TempC,1);
      tempValues = addGraphValue(tempValues, stemp);
      //if (temp < minTemp) minTemp = temp;
      //if (temp > maxTemp) maxTemp = temp;
    }
    else tempValues = addGraphValue(tempValues, "N/A");
     
    if (BM_sensor_status ) {
      //float press = getPressure();
      String spress = String(Press,1);
      pressValues = addGraphValue(pressValues, spress);
      //if (Press < minPress) minPress = Press;
      //if (Press > maxPress) maxPress = Press;
    } 
    else pressValues = addGraphValue(pressValues, "N/A");

    if ( AHTstatus || BMEstatus ) {
      //float Hum = getHum();
      String sHum = String(Hum);
      HumValues = addGraphValue(HumValues, sHum);
      //if ( Hum < minHum || minHum == 0 ) minHum = Hum;
      //if ( Hum > "maxHum ) maxHum = Hum;
      }
    else HumValues = addGraphValue(HumValues, "N/A");

    */
    
    if ( meteo == "ok" ) {


      aprs_latlong();           // calcola lat long per APRS
      String head = meteo_call + ">" + String(DESTCALL_meteo);
      String Tmp_Hum = String(getHumAPRS()); if (Tmp_Hum == "100") Tmp_Hum = "00";
      String Tmp = ":!" + String(meteo_lat_APRS) + "/" + String(meteo_lon_APRS) + "_.../...g..." + "t" + String(getTempAPRS()) + "h" + Tmp_Hum + "b" + String(getPressureAPRS());
      
      if ( meteo_mode == "ip" ) {
        if ( Zambretti == "ok" && forecast != "") aprsis_send(head + Tmp + " previsione: " + forecast); 
        if ( Zambretti != "ok" || forecast == "" ) aprsis_send(head + Tmp + " " + meteo_info);
      }   
      
      if ( meteo_mode == "rf" || meteo_mode == "rf_sleep" ) {  
        if ( Zambretti == "ok" && forecast != "" ) lora_send(head + ",WIDE1-1" + Tmp + " previsione: " + forecast);
        if ( Zambretti != "ok" || forecast == "" ) lora_send(head + ",WIDE1-1" + Tmp + " " + meteo_info);
      }  
        
      //--- dopo il pacchetto meteo se igate e digipeater spenti 

      if ( telemetry == "ok" )  { 
        lastIgBeacon = millis() - int(beacons_timing.toInt() * 60000);   //--- se c'é telemetria re-innesca beacon_igate per emissione pacchetto igate con telemetria 
        lastStMtBeacon = millis();                                       //--- se c'é telemetria evita pacchetto status 
      } 
      if ( telemetry != "ok" ) lastStMtBeacon = millis() - int(beacons_timing.toInt()*24 * 60000);   //--- se non c'é telemetria pacchetto status 
    }

    if ( wunder_switch == "ok" && WiFi.status() == WL_CONNECTED ) wunder_send(); 
    /*
                    if (wunderSendStatus == 1 ) client.print("OK ]");
                    if (wunderSendStatus == 9 ) client.print("KO ]");
    
    */
    
    lastMtBeacon=millis();  
  }
}


void beacon_meteo_status() {    //-------------------------------- beacon dello status meteo
  
  if ( token_tx ) {
  
    if ( meteo == "ok" ) {
      //save_MinMax();

      String Tmp1;
      if ( meteo_mode == "rf" || meteo_mode == "rf_sleep" ) Tmp1 = ",WIDE1-1:>";
      if ( meteo_mode == "ip" ) Tmp1 = ":>";  
    
      String Tmp = meteo_call + ">" + String(DESTCALL_meteo) + Tmp1 + String(DEFAULT_STATUS); 
      if ( telemetry != "ok" && battery_view == "ok" ) {
        Tmp = Tmp + " batt: " + String(voltage_dashboard) + "V.";
        //if ( tlm_aux == "ok" ) Tmp = Tmp + " | aux: " + String(tlm_voltage*voltmeter_param_b) + "V.";
      }
      
      if ( meteo_mode == "rf" || meteo_mode == "rf_sleep"  ) {
        lora_send(Tmp);
        if ( meteo_mode == "rf_sleep" ) start_sleep();
      } 
      
      if ( meteo_mode == "ip" ) aprsis_send(Tmp); // manda gruppo stringhe telemetria in APRS-IS
    }  

    lastStMtBeacon=millis(); 
  } 
}


void beacon_igate_status() {    //-------------------------------- beacon dello status igate
  
  if ( token_tx ) {
    String Tmp = igate_call + ">" + String(DESTCALL_digi) + ":>" + String(DEFAULT_STATUS);
    if ( igate == "ok" ) aprsis_send(Tmp); // se wifi ok e igate acceso manda status in APRS-IS
    Tmp = igate_call + ">" + String(DESTCALL_digi) + ",WIDE1-1:>" + String(DEFAULT_STATUS);
    if ( digi == "ok" ) lora_send( Tmp);
    
    Tmp = ""; 
    lastStIgBeacon=millis();   // azzera contatore dell'invio solo dopo aver inviato le 5 stringhe - azzera anche contatore del ciclo di invio
  }  

} 


void beacon_telemetry_param() { 

  if ( token_tx ) {
    String(telem_param0);
    String(telem_param1);
    String(telem_param2);

  if ( igate == "ok" || digi == "ok") { 
    if (!INA226_status ) { 
      telem_param0 = String(telem0_param0); // telemetria standard senza modulo
      telem_param1 = String(telem0_param1); // telemetria standard senza modulo
      telem_param2 = String(telem0_param2); // telemetria standard senza modulo
    }
    if ( INA226_status ) { 
      telem_param0 = String(telem1_param0); // telemetria standard se modulo INA226 montato
      telem_param1 = String(telem1_param1); // telemetria standard se modulo INA226 montato
      telem_param2 = String(telem1_param2); // telemetria standard se modulo INA226 montato
    }
  }  

  if ( igate != "ok" && digi != "ok" && meteo == "ok" ) { 
    if (!INA226_status ) { 
      telem_param0 = String(telem2_param0); // solo meteo telemetria senza modulo
      telem_param1 = String(telem2_param1); // solo meteo telemetria senza modulo
      telem_param2 = String(telem2_param2); // solo meteo telemetria senza modulo
    }
    if ( INA226_status ) { 
      telem_param0 = String(telem3_param0); // solo meteo telemetria con modulo INA226 montato
      telem_param1 = String(telem3_param1); // solo meteo telemetria con modulo INA226 montato
      telem_param2 = String(telem3_param2); // solo meteo telemetria con modulo INA226 montato
    }
  } 


    String tmp_path; 
    if ( igate == "ok" || meteo_mode == "ip" ) tmp_path = "::";
    if ( digi == "ok" || meteo_mode == "rf" || meteo_mode == "rf_sleep" ) tmp_path = ",WIDE1-1::";   
    
    String Tmp_callsign = igate_call;
    String Tmp_destcall = String(DESTCALL_digi);

    if ( digi != "ok" && igate != "ok" && meteo == "ok" ) {
      Tmp_callsign = meteo_call;
      Tmp_destcall = String(DESTCALL_meteo);
    } 
    
    String xx; 
    if ( Tmp_callsign.length() == 5 ) xx="    ";
    if ( Tmp_callsign.length() == 6 ) xx="   ";
    if ( Tmp_callsign.length() == 7 ) xx="  ";
    if ( Tmp_callsign.length() == 8 ) xx=" ";
  
    String Tmp;
    if ( cnt_param == 0 ) Tmp = Tmp_callsign + ">" + Tmp_destcall + tmp_path + Tmp_callsign + xx + telem_param0;
    if ( cnt_param == 1 ) Tmp = Tmp_callsign + ">" + Tmp_destcall + tmp_path + Tmp_callsign + xx + telem_param1;
    if ( cnt_param == 2 ) Tmp = Tmp_callsign + ">" + Tmp_destcall + tmp_path + Tmp_callsign + xx + telem_param2;
    if ( cnt_param == 3 ) Tmp = Tmp_callsign + ">" + Tmp_destcall + tmp_path + Tmp_callsign + xx + String(telem_param3);
        
    if ( igate == "ok" ) aprsis_send(Tmp);                   // manda gruppo stringhe telemetria in APRS-IS
    else if ( meteo_mode == "ip" ) aprsis_send(Tmp);         // manda gruppo stringhe telemetria in APRS-IS
        else if ( meteo_mode == "rf" ) lora_send(Tmp);    // manda gruppo stringhe telemetria in RF
            else if ( meteo_mode == "rf" ) lora_send(Tmp);  // manda gruppo stringhe telemetria in RF

    cnt_param++; 
    Tmp = ""; 

    if ( cnt_param  >= 4 ) {  
      lastTlParamBeacon=millis();   // azzera contatore dell'invio solo dopo aver inviato le 5 stringhe
      //generateEncodedTelemetry();
      lastIgBeacon = millis() - int(beacons_timing.toInt() * 60000);       //--- invio immediato nuova stringa di geo-posizionamento [ + telemetria ]
      cnt_param = 0; 
    } 
  }    
}
