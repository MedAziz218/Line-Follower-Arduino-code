
#include <WiFi.h>
#include <ArduinoOTA.h>

WiFiClient client;
TaskHandle_t Task1;
TaskHandle_t Task2;

const char* ssid = "TOPNET_89B0";
const char* password = "dr7nmh836k";
bool WiFi_Connected = 0;


void Task2code(void * parameter){
  
  Serial.print("OTA program running on core ");
  Serial.println(xPortGetCoreID());
  
  Serial.println("Connected to WiFi");
  String buff;
  
  while (!client.connect("192.168.1.14", 8000))
    {Serial.println("failed to connect retrying"); delay(1);}
  Serial.println("Connected to server");

  client.println("aya winek");
  while(!client.available())
    {Serial.println("Waiting for response"); delay(1);}
  buff = client.readString();
  Serial.println("He respondeedd");
  Serial.println(buff);
  
  Serial.println("ATTEMPT2");

  while (!client.connect("192.168.1.14", 8000))
    {Serial.println("failed to connect retrying"); delay(1);}
  Serial.println("Connected to server");
  client.println("aya winek");
  while(!client.available())
    {Serial.println("Waiting for response"); delay(1);}

  Serial.println("He respondeedd");
  buff = client.readString();
  Serial.println(buff);
  
  vTaskDelete(Task2);
}
    
void Task1code(void * parameter){

  ArduinoOTA.setHostname("LAMPA2.0");
  ArduinoOTA.begin();
  ArduinoOTA.setPassword("1234");


  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  }); 
  ArduinoOTA.onProgress([](unsigned int, unsigned int) {
    Serial.print(".");
  });
   ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
    ESP.restart();
  });
  //Start Waiting for invintation 
  for (;;){ArduinoOTA.handle();}

  // ArduinoOTA.onError([](ota_error_t error) {
  //   Serial.printf("Error[%u]: ", error);
  //   if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
  //   else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
  //   else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
  //   else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
  //   else if (error == OTA_END_ERROR) Serial.println("End Failed");
  // });


}

void setup() {
  Serial.begin(115200);
  Serial.println("Booting...");
  Serial.print("Arduino program running on core ");
  Serial.println(xPortGetCoreID());

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting wabwoubbb");
  }

  xTaskCreatePinnedToCore(
      Task1code, /* Function to implement the task */
      "Task1", /* Name of the task */
      10000,  /* Stack size in words */
      NULL,  /* Task input parameter */
      0,  /* Priority of the task */
      &Task1,  /* Task handle. */
      0); /* Core where the task should run */

  xTaskCreatePinnedToCore(
      Task2code, /* Function to implement the task */
      "Task2", /* Name of the task */
      10000,  /* Stack size in words */
      NULL,  /* Task input parameter */
      0,  /* Priority of the task */
      &Task2,  /* Task handle. */
      0); /* Core where the task should run */

}

void loop() {
  // your code here
  
  
}
