#pragma once

#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WebSerialLite.h>
#include "main.h"

void WebSerial_setup();
void serial_print(String);
// template <typename T> void serial_print(T message);