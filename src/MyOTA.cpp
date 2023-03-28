#include "MyOTA.h"

volatile bool IS_FLASHING = 0;

void OTA_handle(void * parameter) {
  while(1) {
    ArduinoOTA.handle();
    if (!IS_FLASHING) vTaskDelay(1);
  }
}

void OTA_setup(){
  // if (! IS_WIFI_CONNECTED) {
  //   serial_print("Can t initialiwe OTA\n");
  //   return;
  // }
  // Initialize ArduinoOTA
  ArduinoOTA.begin();
  ArduinoOTA.setHostname("ZARBOUT");
  ArduinoOTA.setPassword("1234");
  //ArduinoOTA.setRebootOnSuccess(1);
  ArduinoOTA.onStart([]() {
    serial_print("\nStarted");
    IS_FLASHING = 1;
  });
  ArduinoOTA.onEnd([]() {
    serial_print("\nEnd");
    ESP.restart();

    // ESP.restart();
  });
  // Set the OTA update task to run on core 0
  xTaskCreatePinnedToCore(
    OTA_handle,     // OTA task function
    "OTA_handle",   // Task name
    10000,          // Stack size
    NULL,           // Task parameters
    1,              // Priority
    NULL,           // Task handle
    0              // Core ID (0 for core 0, 1 for core 1)
  );
}