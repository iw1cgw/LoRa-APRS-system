#include "meteo_functions.h"


void wunder_send() {            // https://support.weather.com/s/article/Managing-Your-Data-Rights?language=en_US&subcategory=Personal_Weather_Stations&type=wu
  if (( BM_sensor_status ) ) {  // https://weatherstation.wunderground.com/weatherstation/updateweatherstation.php?ID=IMONDO21&PASSWORD=ywz1K6rv&dateutc=now&humidity=59&action=updateraw

    String wunder_data;
    wunder_data = "http://weatherstation.wunderground.com/weatherstation/updateweatherstation.php?ID=" + wunder_id + "&PASSWORD=" + wunder_key + "&dateutc=now&tempf=" + String(getTempAPRS()) + "&baromin=" + String(Press*0.02953f);
    
    if (AHTstatus || BMEstatus ) wunder_data = wunder_data + "&dewptf=" + String((DewPoint * 9/5) + 32) + "&humidity=" + String(getHumAPRS());
  
    wunder_data = wunder_data + "&softwaretype=IW1CGW_LoRa_Meteostation&action=updateraw";
    
    wunderSendStatus=0;
    HTTPClient wunder;
    wunder.begin(wunder_data);
    delay(500);

    Serial.print("beacon WUnder: ");
    int httpCode = wunder.GET();
    if (httpCode == 200) {
      String reply = wunder.getString();
      
      if(reply.indexOf("success") != -1 ) {
        wunderSendStatus=1;
        Serial.println("success");
      }
    }
    else {
      Serial.println("error");
      wunderSendStatus=9;
    }
    wunder.end();
  } 
} 


void ZambrettiRoutine() {
  float pressureActual = Press;
  float zFloating;
  
  //Serial.print("pressureTrendReference: ");Serial.println(pressureTrendReference);
  //Serial.print("pressureActual: ");Serial.println(pressureActual);
 
  pressureTrend = "steady";
  zFloating = 144 - 0.13 * pressureActual;    //--- calcolo per "steady"
      if (zFloating <= 11) weatherSymbol =  "&#9728;";
      else if (zFloating <= 12) weatherSymbol =  "&#9925;";
      else if (zFloating <= 15) weatherSymbol = "&#127782;";
      else if (zFloating <= 18) weatherSymbol =  "&#127783;";
      else if (zFloating > 18) weatherSymbol =  "&#127785;";
      else weatherSymbol =  "&#128269;";

  if (pressureTrendReference + 1.6 < pressureActual) {
    pressureTrend = "rising";
    zFloating = 185 - 0.16 * pressureActual;
    if (zFloating <= 22) weatherSymbol = "&#9728;";
    else if (zFloating <= 25) weatherSymbol = "&#9925;";
    else if (zFloating <= 30) weatherSymbol = "&#127782;";
    else if (zFloating > 30) weatherSymbol = "&#127785;";
    else weatherSymbol = "&#128269;";
  }

  if (pressureTrendReference - 1.6 > pressureActual) {
    pressureTrend = "falling";
    zFloating = 127 - 0.12 * pressureActual;
    if (zFloating <= 2) weatherSymbol = "&#9728;";
    else if (zFloating <= 4) weatherSymbol = "&#9925;";
    else if (zFloating <= 7) weatherSymbol = "&#127782;";
    else if (zFloating > 7) weatherSymbol = "&#127783;";
    else weatherSymbol = "&#128269;";
  }

  forecast = "";
  if ( pressureTrend == "falling" ) forecast = "peggioramento, ";
  if ( pressureTrend == "rising" ) forecast = "miglioramento, ";
  if (zFloating > 0 && zFloating <= 4) forecast += "sereno";
  if (zFloating >4 && zFloating <= 5) forecast += "nubi sparse";
  if (zFloating >5 && zFloating <= 8) forecast += "instabilita, pioggia";
  if (zFloating >8 && zFloating <= 9) forecast += "molto instabile, pioggia";

  if (zFloating > 9 && zFloating <= 12) forecast += "sereno";
  if (zFloating > 12 && zFloating <= 13) forecast += "nuvolosita in aumento";
  if (zFloating > 13 && zFloating <= 14) forecast += "nuvolosita brevi pioggie e schiarite";
  if (zFloating > 14 && zFloating <= 15) forecast += "nuvoloso, instabile, possibilità piovaschi";
  if (zFloating > 15 && zFloating <= 16) forecast += "nuvoloso, instabile, piovaschi";
  if (zFloating > 16 && zFloating <= 17) forecast += "pioggia a intervalli frequenti";
  if (zFloating > 17 && zFloating <= 18) forecast += "molto instabile pioggia";
  if (zFloating > 18 && zFloating <= 19) forecast += "pioggia intensa";
  if (zFloating > 19 && zFloating <= 20) forecast += "";
  if (zFloating > 20 && zFloating <= 22) forecast += "sereno";
  if (zFloating > 22 && zFloating <= 23) forecast += "poche nubi";
  if (zFloating > 23 && zFloating <= 25) forecast += "nubi, possibili scrosci";
  if (zFloating > 25 && zFloating <= 26) forecast += "nuvolovo, instabilità";
  if (zFloating > 26 && zFloating <= 27) forecast += "piuttosto nuvoloso";
  if (zFloating > 27 && zFloating <= 28) forecast += "nuvoloso, instabile";
  if (zFloating > 28 && zFloating <= 29) forecast += "nuvoloso, instabile";
  if (zFloating > 29 && zFloating <= 30) forecast += "molto nuvoloso, instabile";
  if (zFloating > 30 && zFloating <= 31) forecast += "temporali";
  if (zFloating >31 ) forecast += "forti temporali";

  /*
  if (weatherSymbol == "&#128269;" ) forecast = "";
  if (weatherSymbol == "&#9728;" ) forecast = "bel tempo";
  if (weatherSymbol == "&#9925;" ) forecast = "nuvoloso";
  if (weatherSymbol == "&#127782;" ) forecast = "peggioramento, pioggia";
  if (weatherSymbol == "&#127783;" ) forecast = "maltempo, pioggia";
  if (weatherSymbol == "&#127785;" ) forecast = "maltempo, temporali";
  */

  //Serial.print(F("trend: "));Serial.println(pressureTrend );
  //Serial.print(F("valore di zFloating: "));Serial.println( zFloating );
  //Serial.print(F("weatherSymbol: "));Serial.println(weatherSymbol);

  pressureTrendReference = pressureActual;
  pressureTrendTimeout = millis();
}
