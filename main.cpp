#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

int P;
int I;
int D;
float Pvalue;
float Ivalue;
float Dvalue;

int lastError = 0;

bool BLACK = 1;
bool WHITE = 0;
const uint8_t SensorCount = 8;
int SensorPins[SensorCount] = { 2, 3, 4, 5, A0, A1, A2, A3 };
int SensorWeights[SensorCount] = { -80, -50, -30, -10, 10, 30, 50, 80 };
int SensorRawValues[SensorCount];

#define maxspeeda 255
#define maxspeedb 255

#define minspeeda 0
#define minspeedb 0

#define multiP 1
#define multiI 4
#define multiD 2

int basespeeda;
int basespeedb;
int turnspeed;

int Kp = 16;  // 14
int Ki = 1;
int Kd = 10;


int enbA = 6;   //enbA
int inpA1 = 8;  //inp1
int inpA2 = 7;  //inp2

int enbB = 11;   //enbB
int inpB1 = 10;  //inp3
int inpB2 = 9;   //inp4

// 0:default
// 1:speed 1
// 2:speeeeed 2
void gear_box(int gear) {

  //default
  switch (gear) {

    case 0:
      basespeeda = 180;  //a is right
      basespeedb = 180;  //b is left
      turnspeed = 180;
      break;
    case 1:
      basespeeda = 259;  //a is right
      basespeedb = 259;  //b is left
      turnspeed = 180;
      break;

    case -1:
      basespeedb = 120;
      basespeeda = 120;
      turnspeed = 120;
      break;
  }
}

void setup() {
  int lunch_time,current_time;
  //Serial.begin(9600);
 
  pinMode(enbA, OUTPUT);
  pinMode(inpA1, OUTPUT);
  pinMode(inpA2, OUTPUT);

  pinMode(enbB, OUTPUT);
  pinMode(inpB1, OUTPUT);
  pinMode(inpB2, OUTPUT);

  for (int i = 0; i < SensorCount; i++) {
    pinMode(SensorPins[i], INPUT);
  }

  pinMode(LED_BUILTIN, OUTPUT);
  
  gear_box(0);
  pinMode(LED_BUILTIN, OUTPUT);
  lcd.init();  
  lcd.noBacklight();
  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(3, 0);
  pinMode(12,INPUT);
  current_time=millis();
  int x=0;
  digitalWrite(LED_BUILTIN, HIGH);
  while(1){
  x=digitalRead(12);
  //Serial.println(x);
    if(x==HIGH){
        lunch_time=millis();
      if(lunch_time-current_time>200)
      break;
    }
  }
  digitalWrite(LED_BUILTIN, LOW);
  delay(200);
  forward_brake(basespeeda, basespeedb);
  //lcd.noBacklight();
  //lcd.print("Gryffindor");
 
}

void loop() {
  robot_control();
}

bool invert = 0;
int checkpoint =-1; //-1;
int counter = 0;
void next_checkpoint() {
  checkpoint++;
  counter = 0;
}
unsigned long END_OF_TIMER = 0;

void robot_control() {
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
  //Serial.print("cnt:");Serial.print(cnt);Serial.print(" val:");Serial.println(val);

  // ---- El Maquette ---- \\ 

  if (checkpoint == -1){
    start_timer(200);
    next_checkpoint();
  }
  else if (checkpoint == 0){
        if (   (  (SensorRawValues[7] || SensorRawValues[6]) && (counter == 0) && is_stopped() ) || counter == 1) 
              { //-> Entering First Turn
              
                
                if (counter == 0){counter ++ ; start_timer(200);right_brake(turnspeed/2,turnspeed); all_good = 1;gear_box(-1); }
                else if (counter == 1 && !is_stopped()){right_brake(turnspeed/2,turnspeed); all_good = 1;}
                else if (counter == 1 && is_stopped()){ counter++; start_timer(200);  }


              } 
        else if ((SensorRawValues[7] || SensorRawValues[6]) && (counter == 2) && is_stopped())
              { //-> Exiting the turn 
              /// WAITING TIME BEFORE STOP
              // B4STOP
              start_timer(650); counter ++ ;
              }
        else if ( (counter== 3) && is_stopped()  )
              { //-> Stopping
                Serial.println("Stopping");
                backward_brake(basespeeda, basespeedb);
                delay(20);
                forward_brake(0,0);
                gear_box(0);
                lcd.clear();
                lcd.backlight();
                lcd.print("Gryffindor");
                counter ++;
                delay(4800);
                lcd.noBacklight();
               
                next_checkpoint(); start_timer(100);
                
              }
  }
  else if (checkpoint == 1){
            
           
        if (   ( ((cnt >= 3 && val >= 5.5)) && (counter == 0) && is_stopped() ) || counter == 1) 
              { //-> passing through second Turn
                if (counter == 0){counter ++ ; start_timer(250);right_brake(turnspeed,turnspeed); all_good = 1;gear_box(-1); }
                else if (counter == 1 && !is_stopped()){right_brake(turnspeed,turnspeed); all_good = 1;}
                else if (counter == 1 && is_stopped()){ counter++; start_timer(200);  }

              } 
        else if (  (  (cnt >= 5) && (counter == 2) && is_stopped() ) || counter == 3) 
              { //-> passing through third Turn
                
                if (counter ==2 ){counter ++ ; start_timer(200);right_brake(turnspeed/2,turnspeed); all_good = 1;gear_box(-1); }
                else if (counter == 3 && !is_stopped()){right_brake(turnspeed/2,turnspeed); all_good = 1;}
                else if (counter == 3 && is_stopped()){ counter++; start_timer(200);  }

              } 
        else if ( counter == 4 && is_stopped())
              { //-> finished this part restore the speed
                next_checkpoint(); start_timer(300);gear_box(0);
              }
   }      
  else if (checkpoint == 2){
        
        if (counter == 0 && is_stopped())
              { //-> up the speeeeeed
                counter++;
                gear_box(1); 
                start_timer(300);
              } 
        else if ( (counter == 1  && is_stopped() )|| counter == 2)
              {//-> reached the black arrow and  handling it
                if ( (cnt >= 4) && counter == 1) { cnt = 0 ; val = 3.5;error=0; counter++;}
                else if ( (cnt >= 3) && counter ==2) {cnt = 0 ; val = 3.5; error = 0;}//forward_brake(basespeeda, basespeedb); }
                else if (counter == 2) {counter++; start_timer(200);}
              
              }
        else if ( (counter == 3 && is_stopped()) ) 
              {
                
                gear_box(0); counter++;start_timer(100) ; 
                
              }
        else if ( ( (cnt>=4 & val>3.5) && (counter == 4) && is_stopped() ) || counter == 5) 
              { //->reached the BITCH turn and handling it 

                
                if (counter == 4){counter ++ ; start_timer(600);right_brake(turnspeed/2,turnspeed); all_good = 1;gear_box(-1); }
                else if (counter == 5 && !is_stopped()){right_brake(turnspeed/2,turnspeed); all_good = 1;}
                else if (counter == 5 && is_stopped()){ counter++; start_timer(200);gear_box(-1);next_checkpoint() ;}


              } 
                 
  }
  else if (checkpoint == 3) {
        //---- force turn block ----//
        if ((cnt==0) && (counter == 0 ) && is_stopped())
              { 
                right_brake(turnspeed/2,turnspeed);
                start_timer(50);
                //gear_box(0);
                counter++;
                all_good = 1;
                
               //stop_motors(5000);
              }
        else if (counter == 1 )
              { 
                
                if (is_stopped() || cnt>0){ counter ++; reset_timer(); start_timer(200);}
                else {all_good = 1;right_brake(turnspeed/2,turnspeed);}
              } 
        //---- end force turn ----//

        else if (((SensorRawValues[7])&&(SensorRawValues[0])==1) && (counter == 2) && is_stopped())
              {
                Serial.println("inverting sensors");
                
                start_timer(300);
                
                invert = (! invert) ;
                if (invert == 0){counter ++; start_timer(1000);}

              }
        else if ( (cnt >=4 ) && counter == 3)
              {
                
               counter ++;
              }
        else if (counter == 4)
              {
                
                // forward_brake(0, 0);
               //all_good = 1;
              }
    
  }
  
  // update timer 
  if (is_time() & (!is_stopped())) { reset_timer();  }

  // fallback code 
  if (all_good == 0) {
    if ((cnt >= 4 && val > 3.5) || (cnt >= 3 && val >= 5.5))
        {
          right_brake(turnspeed / 2, turnspeed);
        }
    else if ((cnt >= 4 && val < 3.5) || (cnt >= 3 && val <= 2.5)) 
        {
          left_brake(turnspeed, turnspeed / 2);
        }
    else 
        {
          PID(error);
        }
  }
  
  // // update timer
  // if (is_time() & (! is_stopped())){
  //   reset_timer();
  // }

  // // fallback code 
  // if (all_good == 0)
  //   {
     
  //   if ((cnt>=4 && val>3.5) || (cnt >= 3 && val >5.5 ))
  //       {
  //         right_brake(turnspeed/2, turnspeed);  
  //       }
  //   else if ( (cnt>=4 && val<3.5) || (cnt >=3 && val < 2.5  )) 
  //       {
  //         left_brake(turnspeed, turnspeed/2);
  //       }
  //   else 
  //       {
  //        PID(error);
  //       }
  
  //   }         

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
  //Serial.print(" right: "); Serial.print(motorspeeda);Serial.print(" left: ");Serial.println(motorspeedb); Serial.println();
  // delay(500);
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

  analogWrite(enbA, posa);
  analogWrite(enbB, posb);
}
//Wheel A moves Forward, Wheel B stopsSensorValues
void left_brake(int posa, int posb) {
  digitalWrite(inpA1, HIGH);
  digitalWrite(inpA2, LOW);
  digitalWrite(inpB1, LOW);
  digitalWrite(inpB2, HIGH);

  analogWrite(enbA, posa);
  analogWrite(enbB, posb);
}
//Wheel A stops, Wheel B moves Forward
void right_brake(int posa, int posb) {
  digitalWrite(inpA1, LOW);
  digitalWrite(inpA2, HIGH);
  digitalWrite(inpB1, HIGH);
  digitalWrite(inpB2, LOW);

  analogWrite(enbA, posa);
  analogWrite(enbB, posb);
}
void backward_brake(int posa, int posb) {
  digitalWrite(inpA1,LOW );
  digitalWrite(inpA2,HIGH );
  digitalWrite(inpB1,LOW );
  digitalWrite(inpB2,HIGH );

  analogWrite(enbA, posa);
  analogWrite(enbB, posb);
}
void stop_motors(int dt) {
  forward_brake(0, 0);
  delay(dt);
}



// ### Timer and Shit ## //
//start new Timer
void start_timer(unsigned long duration) {
  if (is_stopped()) {
    END_OF_TIMER = millis() + duration;
  }
}
//Timer is stopped
bool is_stopped() {
  return (END_OF_TIMER == 0);
}
//Reset the timer
bool reset_timer() {
  END_OF_TIMER = 0;
}
//Check if the timer is done
bool is_time() {
  //timer isn t even started
  if (is_stopped()) return 0;

  //timer has ended
  if ((END_OF_TIMER <= millis())) return 1;

  return 0;
}



