#define PWM_Ch1      0
#define PWM_Ch2      1
#define PWM_Res      8
#define PWM_Freq  1000

int inpA1 = 17;
int inpA2 = 5;
int enbA = 18;

int inpB1;
int inpB2;
int enbB = 19;

int LED_PIN = 17;
int PWM1_DutyCycle = 0;
volatile int count =0;
#include "main.h"

bool isAllNumbers(String str) {
    for (char c : str) {
        if (!isdigit(c)) {
            return false;
        }
    }
    return true;
}
void serial_control(String input){
  // WebSerial.print(input+"\n");
  Serial.print(input+"\n");
  if (input == "r" || input=="reboot"){
    serial_print("rebooting");
    vTaskDelay(200);
    ESP.restart();}
  
  if (input == "tasks"){ 
    Serial.print("running tasks: ");
    // Serial.println(uxTaskGetNumberOfTasks());
  }
  if  (!isAllNumbers(input) ) return;
  // LED_PIN = input.toInt();  serial_print(">> setting GPIO = "+input);  pinMode(LED_PIN,INPUT); pinMode(LED_PIN,OUTPUT);
  // serial_print(">> setting duty cycle = "+input);

  PWM1_DutyCycle =  input.toInt();
  ledcWrite(PWM_Ch1, PWM1_DutyCycle);
  // ledcWrite(PWM_Ch2, PWM1_DutyCycle);
  
}

hw_timer_t *Timer0_Cfg = NULL;
long unsigned int run_counter = 0;
void IRAM_ATTR Timer0_ISR()
{
    Serial.print(">> ");
    Serial.println(run_counter);
}
void setup()
{
  Serial.begin(115200);Serial.println("Booting MAN...");
  WiFi_setup(1); WebSerial_setup(); OTA_setup();

  pinMode(inpA1,OUTPUT); 
  pinMode(inpA2,OUTPUT);
  
  // pinMode(inpA2,INPUT_PULLUP);
  digitalWrite(inpA1,HIGH);
  digitalWrite(inpA2,LOW);

  ledcSetup(PWM_Ch1, PWM_Freq, PWM_Res);
  ledcSetup(PWM_Ch2, PWM_Freq, PWM_Res);
  ledcAttachPin(enbA, PWM_Ch1);
  ledcAttachPin(enbB, PWM_Ch2);


  ledcWrite(PWM_Ch1, PWM1_DutyCycle);
  ledcWrite(PWM_Ch2, PWM1_DutyCycle);

  // testing new shit
  Timer0_Cfg = timerBegin(0, 80, true);
    timerAttachInterrupt(Timer0_Cfg, &Timer0_ISR, true);
    timerAlarmWrite(Timer0_Cfg, 1000, true);
    // timerAlarmEnable(Timer0_Cfg);


}
#include<string>  
using namespace std;

void loop()
{
  uint32_t Freq = 0;
 Freq = getCpuFrequencyMhz();
  Serial.print("CPU Freq = ");
  Serial.print(Freq);
  Serial.println(" MHz");
  Freq = getXtalFrequencyMhz();
  Serial.print("XTAL Freq = ");
  Serial.print(Freq);
  Serial.println(" MHz");
  Freq = getApbFrequency();
  Serial.print("APB Freq = ");
  Serial.print(Freq);
  Serial.println(" Hz");
  vTaskDelay(1000);
}
// void setup() {
//   Serial.begin(115200);

//   Serial.println("Booting MAN...");

//   WiFi_setup(1);
//   WebSerial_setup();
//   OTA_setup();

//   // Create WiFi network
//   // Initialize OTA update
//   // WiFi.softAP(ssid, password);
//   // ArduinoOTA.begin();
  
//   pinMode(LED_PIN,OUTPUT);

// }

// void loop() {

//   vTaskDelay(300);
//   digitalWrite(LED_PIN,HIGH);

//   // serial_print(">> Bola");
//   vTaskDelay(300);
//   digitalWrite(LED_PIN,LOW);


  

//   // Your code here.../*  */
// }
