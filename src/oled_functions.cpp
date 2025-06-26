#include "oled_functions.h"
#include <Arduino.h> // necessario per String, float, ecc.


void make_meteo_display()
{
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  byte sep = 0;

  float temp = TempC;
  String stemp = String(temp, 1);
  String spressd = String(Press, 0);
  String sHum = String(Hum);

  display.setCursor(0, sep);
  display.print("Temperature:");
  display.print(stemp);
  display.println(" C");
  sep += 11;

  display.setCursor(0, sep);
  display.print("Pressure   :");
  display.print(spressd);
  display.println(" hPa");
  sep += 11;

  display.setCursor(0, sep);
  display.print("Humidity   :");
  display.print(sHum);
  display.println(" %");
  sep += 11;

  display.setCursor(0, sep);
  display.println("---------------------");
  sep += 11;

  display.setCursor(0, sep);
  display.print(meteo_call);
  sep += 11;

  display.setCursor(0, sep);
  if ( myip == "192.168.4.1") display.print("AP ip: ");
  else display.print("ip: ");
  display.print(myip);

  display.display();
}


void make_display()
  {
    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setTextSize(1);
    byte sep = 0;
    display.setCursor(0,sep);
    display.print("iGate/digi:");
    display.print(igate_call);
    sep=sep+9;
    display.setCursor(0,sep);
    display.print("iGate:");
    if (igate == "ok")display.print("ON");
    if (igate != "ok") display.print("OFF");
    if (igate_backup != "ok") display.print(" - digi:");
    if (igate_backup == "ok") display.print(" * digi:");
    if (digi == "ok")display.print("ON");
    if (digi != "ok") display.print("OFF");

    //if (Xmode && Experimental) display.println(" X");
    //else display.println("");
    sep=sep+9;
    display.setCursor(0,sep);
    display.print("meteo send ");
    if (meteo != "ok" ) display.println("OFF");
    if (meteo == "ok" ) {
      if (meteo_mode == "rf" || meteo_mode == "rf_sleep" ) display.println("to RF");
      if (meteo_mode == "ip" ) display.println("to ip");
    }
    sep=sep+9;
    display.setCursor(0,sep);
    display.print("telemetry:");
    if (telemetry == "ok")display.print("ON");
    if (telemetry != "ok") display.print("OFF");
    
    sep=sep+9;
    display.setCursor(0,sep);
    display.println("---------------------");
    sep=sep+9;
    display.setCursor(0,sep);
    display.print(igate_call);  
    sep=sep+9;
    display.setCursor(0,sep); 
    if ( myip == "192.168.4.1") display.print("AP ip: ");
    else display.print("ip: ");
    display.print(myip);
    display.display();  
  }


void display_info(String info1, String info2) {
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(10,10);
  display.print(info1);
  display.setCursor(10,30);
  display.print(info2);
  display.display(); 
  delay(2500);
}

