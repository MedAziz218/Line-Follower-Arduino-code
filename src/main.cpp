#define PWM_Ch1      0
#define PWM_Ch2      1
#define PWM_Res      8
#define PWM_Freq  1000

int P;
int I;
int D;
float Pvalue;
float Ivalue;
float Dvalue;

int lastError = 0;
#define SensorCount 10

// int SensorPins[SensorCount] = { 23, 22, 21, 17, 16  ,4, 0, 26, 15, 27 };
int SensorPins[10] = { 23, 22, 21, 19, 18,   5,17, 16, 4, 15  };
int SensorWeights[10] = { -80, -50, -30, -10, 10, 30, 50, 80 };
int SensorRawValues[10];

#define maxspeeda 255
#define maxspeedb 255

#define minspeeda 0
#define minspeedb 0

#define multiP 1
#define multiI 4
#define multiD 2
// 0 for normal; -2 to stop; 11 forward; 22 left
// 33 right; 44 backwards;
int STATE = 0;
int basespeeda = 150;
int basespeedb = 150;
int turnspeed = 150;

int Kp = 16;  // 14
int Ki = 1;
int Kd = 10;


int enbA = 32;
int inpA1 = 25;
int inpA2 = 33;

int enbB = 26;
int inpB1 = 14;
int inpB2 = 27;

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
  
  if (input == "pause" || input == "p"){ STATE = -2;}
  if (input == "start" || input == "s"){ STATE = 0;}
  if (input == "forward" || input == "f"){ STATE = 11 ;}
  if (input == "left" || input == "l"){ STATE = 22 ;}
  if (input == "right" || input == "ri"){ STATE = 33;}
  if (input == "back" || input == "b"){ STATE = 44 ;}

  if (input == "tasks"){ 
    Serial.print("running tasks: ");
    // Serial.println(uxTaskGetNumberOfTasks());
  }
  if  (!isAllNumbers(input) ) return;
  digitalWrite(LED_PIN,LOW);
  // LED_PIN = input.toInt();  serial_print(">> setting GPIO = "+input);  pinMode(LED_PIN,INPUT); pinMode(LED_PIN,OUTPUT);
  // digitalWrite(LED_PIN,HIGH);
  serial_print(">> setting duty cycle = "+input);

  int yis =  input.toInt();
  
  forward_brake(yis,yis);
  
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

  STATE = 1;
  /*--------- setup sensor and motor pinouts ------ */
  
  for (int i = 0; i < SensorCount; i++) {
    pinMode(SensorPins[i], INPUT);
  }

  pinMode(inpA1,OUTPUT); 
  pinMode(inpA2,OUTPUT);
  
  pinMode(inpB1,OUTPUT); 
  pinMode(inpB2,OUTPUT);

 

  ledcSetup(PWM_Ch1, PWM_Freq, PWM_Res);
  ledcSetup(PWM_Ch2, PWM_Freq, PWM_Res);
  ledcAttachPin(enbA, PWM_Ch1);
  ledcAttachPin(enbB, PWM_Ch2);
  ledcWrite(PWM_Ch1, 0);
  ledcWrite(PWM_Ch2, 0);

  // forward_brake(basespeeda,basespeedb);

  //-----------------------------//
  // pinMode(inpA2,INPUT_PULLUP);


}

void loop(){
  if (STATE == 0) robot_control();
  else if (STATE == -2) {stop_motors(0); STATE = -1;}
  else {
    switch (STATE) {
      case 11: forward_brake(basespeeda,basespeedb); vTaskDelay(500/portTICK_PERIOD_MS);STATE = -1; break;
      case 22: left_brake(turnspeed,turnspeed); vTaskDelay(500/portTICK_PERIOD_MS);STATE = -1; break;
      case 33: right_brake(turnspeed,turnspeed); vTaskDelay(500/portTICK_PERIOD_MS); STATE = -1;break;
      case 44: backward_brake(basespeeda,basespeedb);vTaskDelay(500/portTICK_PERIOD_MS);STATE = -1; break;
    }
    
  }
 
}
bool invert = 0;
void robot_control()
{
  bool all_good = 0;
  int cnt = 0;
  float sum = 0;
  float val = 0;
  int error = 0;
  int i;
  for (i = 0; i < SensorCount; i++) {
    //black 1, white 0 , right a , left b
    SensorRawValues[i] = digitalRead(SensorPins[i]);
    if (invert) { SensorRawValues[i] = (!SensorRawValues[i]); }
    if (SensorRawValues[i] == 1) {
      cnt++;
      sum = sum + i;
    }
    error += SensorRawValues[i] * SensorWeights[i];
  };
  if (cnt) val = sum / cnt;
  

 
   // fallback code
   
  
  if (all_good == 0) {
    if ((cnt >= 4 && val > 3.5) || (cnt >= 3 && val >= 5.5))
        {
          forward_brake(turnspeed / 2, turnspeed); serial_print("left"+String(turnspeed));
          
        }
    else if ((cnt >= 4 && val < 3.5) || (cnt >= 3 && val <= 2.5)) 
        {
          forward_brake(turnspeed, turnspeed / 2);serial_print("right");
        }
    else 
        {
          PID(error);
        }
    vTaskDelay(400/portTICK_PERIOD_MS);
  }
 
}

void PID(int error) {
  int P = error;
  int I = I + error;
  int D = error - lastError;
  lastError = error;
  Pvalue = (Kp / pow(10, multiP)) * P;
  Ivalue = (Ki / pow(10, multiI)) * I;
  Dvalue = (Kd / pow(10, multiD)) * D;

  float motorspeed = Pvalue + Ivalue + Dvalue;

  int motorspeeda = basespeeda - motorspeed;
  int motorspeedb = basespeedb + motorspeed;
  //Serial.print(" right: "); Serial.print(motorspeeda);Serial.print(" left: ");Serial.println(motorspeedb);

  motorspeeda = clampi(motorspeeda, minspeeda, maxspeeda);
  motorspeedb = clampi(motorspeedb, minspeedb, maxspeedb);
  // serial_print(" right: "+String(motorspeeda) +" left: "+String(motorspeedb)+"\n");
  
  forward_brake(motorspeeda, motorspeedb);
}
int clampi(int val, int val_min, int val_max) {
  if (val > val_max) {
    val = val_max;
  }
  if (val < val_min) {
    val = val_min;
  }
  return val;
}


//Both Wheels move Forward
void forward_brake(int posa, int posb) {
  digitalWrite(inpA1, HIGH);
  digitalWrite(inpA2, LOW);
  digitalWrite(inpB1, HIGH);
  digitalWrite(inpB2, LOW);

  ledcWrite(PWM_Ch1, posa); //analogWrite(enbA, posa);
  ledcWrite(PWM_Ch2, posb); //analogWrite(enbB, posb);
}
//Wheel A moves Forward, Wheel B stopsSensorValues
void left_brake(int posa, int posb) {
  digitalWrite(inpA1, HIGH);
  digitalWrite(inpA2, LOW);
  digitalWrite(inpB1, LOW);
  digitalWrite(inpB2, HIGH);

  ledcWrite(PWM_Ch1, posa); //analogWrite(enbA, posa);
  ledcWrite(PWM_Ch2, posb); //analogWrite(enbB, posb);
}
//Wheel A stops, Wheel B moves Forward
void right_brake(int posa, int posb) {
  digitalWrite(inpA1, LOW);
  digitalWrite(inpA2, HIGH);
  digitalWrite(inpB1, HIGH);
  digitalWrite(inpB2, LOW);

  ledcWrite(PWM_Ch1, posa); //analogWrite(enbA, posa);
  ledcWrite(PWM_Ch2, posb); //analogWrite(enbB, posb);
}
void backward_brake(int posa, int posb) {
  digitalWrite(inpA1,LOW );
  digitalWrite(inpA2,HIGH );
  digitalWrite(inpB1,LOW );
  digitalWrite(inpB2,HIGH );

  ledcWrite(PWM_Ch1, posa); //analogWrite(enbA, posa);
  ledcWrite(PWM_Ch2, posb); //analogWrite(enbB, posb);
}
void stop_motors(int dt) {
  forward_brake(0, 0);
  if (dt) delay(dt);
}
void print_rawValues(int d){
  String l = "";
  l += "{";
  for (int i=0; i<10;i++) {l+= String(SensorRawValues[i]) +";";};
  l += "}";
  if (d) serial_print(l);
  else Serial.println(l);
}
/*code for PWM*/

  // digitalWrite(inpA1,HIGH);
  // digitalWrite(inpA2,LOW);

  // ledcSetup(PWM_Ch1, PWM_Freq, PWM_Res);
  // ledcSetup(PWM_Ch2, PWM_Freq, PWM_Res);
  // ledcAttachPin(enbA, PWM_Ch1);
  // ledcAttachPin(enbB, PWM_Ch2);


  // ledcWrite(PWM_Ch1, PWM1_DutyCycle);
  // ledcWrite(PWM_Ch2, PWM1_DutyCycle);
