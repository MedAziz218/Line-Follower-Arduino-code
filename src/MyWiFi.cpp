#include "MyWiFi.h"

String IP = "";

// mode : 0 for AP , 1 for client
void WiFi_setup(int mode){
  if (mode == 0) {
    WiFi.softAP(ssid_ap, password_ap);
    IP = WiFi.softAPIP().toString();

  }

  if (mode == 1) {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
      Serial.println("Connection Failed! Rebooting...");
      delay(1000);
      ESP.restart();
    }
    IP = WiFi.localIP().toString();
  }
  
  if (IP.length() == 0)
    {Serial.println("Failed to initialize WiFi");}
  else 
    {Serial.println("Ready...\nIP Address: "+IP +"/webserial");}

}