#pragma once
#include "MyWiFi.h"
#include "MyWebSerial.h"
#include "MyOTA.h"
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"

extern String IP;
extern volatile bool IS_FLASHING;
//########################################################################//
                    /*  mode 0 (ap) settings  */
constexpr auto ssid_ap = "ZARBOUT", password_ap = "yahoo666999"; // Your WiFi Passwordyahoo
          
                    /* mode 1 (client) settings */
// constexpr auto ssid = "Infinix HOT 77", password = "1234567A" ;                    
constexpr auto ssid = "TOPNET_89B0", password = "dr7nmh836k" ;
// constexpr auto ssid = "SC-ENSI", password = "test1234" ;
// constexpr auto ssid = "Fixbox-7E3FE0", password = "NmFkYmQz" ;

void serial_control(String input);

int clampi(int val, int val_min, int val_max);
void PID(int error);
void forward_brake(int posa, int posb);
void left_brake(int posa, int posb);
void right_brake(int posa, int posb);
void backward_brake(int posa, int posb) ;
void stop_motors(int dt);
void print_rawValues(int);
void robot_control();
