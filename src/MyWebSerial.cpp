#include "MyWebSerial.h"

AsyncWebServer server(80);

void recvMsg(uint8_t *data, size_t len){
  // WebSerial.println("Received Data...");
  String d = "";
  for(int i=0; i < len; i++){
    d += char(data[i]);
  }
  // WebSerial.println(d);
  serial_control(d);
}

void WebSerial_setup(){
  WebSerial.begin(&server);
   /* Attach Message Callback */
  WebSerial.onMessage(recvMsg);
  server.begin();
}


void serial_print(String message){
  Serial.println(message);
  WebSerial.println(message);
  
}

// template <typename T>
// void serial_print(T message)
// {
//   bool b = std::is_same<T, const char *>::value ;
//   // if (b) Serial.print(">>");
//   Serial.println(message);
//   WebSerial.println(message);
// }