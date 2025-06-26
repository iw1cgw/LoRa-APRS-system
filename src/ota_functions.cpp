#include "ota_functions.h"
#include "rtc.h"
#include <HTTPClient.h>
#include <AsyncTCP.h>
#include <Update.h>

#include "oled_functions.h"
#include "config.h"

struct tm timeinfo;



//===============================================================================



String urlEncode(const String& str) {
  String encoded = "";
  char c;
  char bufHex[4];
  for (int i = 0; i < str.length(); i++) {
    c = str.charAt(i);
    if (isalnum(c)) {
      encoded += c;
    } else {
      sprintf(bufHex, "%%%02X", c);
      encoded += bufHex;
    }
  }
  return encoded;
}


void OTA_logbook()  {

}


bool NTP_query() {
  if ( WiFi.status() == WL_CONNECTED ) {
    const char* ntpServer = "time.nist.gov";

    //const long  gmtOffset_sec = int(gmt_zone*3600); 
    const long  gmtOffset_sec = int(gmt_zone.toInt()*3600); 

    //const int   daylightOffset_sec = 3600; //  definisce l'offset in secondi per l'ora legale. In genere è un'ora, che corrisponde a 3600 secondi
    const int   daylightOffset_sec = 0; //  definisce l'offset in secondi per l'ora legale. In genere è un'ora, che corrisponde a 3600 secondi

    // Init and get the time
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    delay(1000);
    
    if ( getLocalTime(&timeinfo) ) {
      rtc.setTimeStruct(timeinfo); 

      // 
      //sprintf(NTP_data, "%04d-%02d-%02d %02d:%02d:%02d", timeinfo.tm_year+1900, timeinfo.tm_mon+1, timeinfo.tm_mday, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
      //sprintf(NTP_dataLight, "%02d:%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
      //Serial.print(F("GMT zone: "));Serial.println(GMT_zone);
      //Serial.print(F("NTP_data: "));Serial.println(NTP_data);
      //Serial.print(F("NTP_dataLight: "));Serial.println(NTP_dataLight);
      //

      Serial.println("NTP: OK to obtain time");
      Serial.println("RTC: OK setup ESP32 RTC");
      RTC_status = true;
      return true;
    }

    if( !getLocalTime(&timeinfo) ) {
      Serial.println("NTP: Failed to obtain time");
      return false;
    }
  } return false;
}


bool updateFirmware() {
	return false;
}


bool checkForUpdates() {
	return false;
} 


