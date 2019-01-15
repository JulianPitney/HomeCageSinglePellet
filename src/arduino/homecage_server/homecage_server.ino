#include <Servo.h>

bool SIMULATION_MODE = false;


// Config
const int servo1Pin = 10;
const int servo2Pin = 9;
Servo servo1;
Servo servo2;
bool servo1_up_flag = false;
bool servo2_up_flag = false;
const int SERVO_SETTLE_DELAY = 300;
// Higher numbers make the arm go higher
int SERVO1_UP_POS = 120;
// Low numbers make the arm go lower
int SERVO1_DOWN_POS = 50;
// Lower numbers make the arm go higher
int SERVO2_UP_POS = 57;
// High numbers make the arm go lower
int SERVO2_DOWN_POS = 122;
int SERVO_PULSE_DELAY = 15;
int servo1Pos = SERVO1_DOWN_POS;
int servo2Pos = SERVO2_DOWN_POS;
typedef enum {left,right} whichServo;

const int ledPin = 13;
const int switchPin = 2;
const int IRBreakerPin = 3;
const int ptgreyGPIOSignalPin = A1;

volatile byte switchState = digitalRead(switchPin);
volatile byte IRState;

int stepperDistFromOrigin = -1;
double stepsToMmRatio = 420;


// Hardware interrupt handler for switch pin
void handleSwitchChange() {

  switchState = digitalRead(switchPin);
}

// Hardware interrupt handler for IR breaker pin 
void handleIRChange() {

  IRState = digitalRead(IRBreakerPin);  
  digitalWrite(ledPin, IRState);

}


int zeroServos() {

  servo1.attach(servo1Pin);
  servo2.attach(servo2Pin);


  for (int i = servo1Pos; i >= SERVO1_DOWN_POS; i -= 1) {
      servo1.write(i);
      delay(SERVO_PULSE_DELAY);
    }   
  delay(SERVO_SETTLE_DELAY);
  servo1Pos = SERVO1_DOWN_POS;;

  for (int i = servo2Pos; i <= SERVO2_DOWN_POS; i += 1) {
      servo2.write(i);
      delay(SERVO_PULSE_DELAY);
    }   

  delay(SERVO_SETTLE_DELAY);
  servo2Pos = SERVO2_DOWN_POS;

  servo1.detach();
  servo2.detach();
  return 0;

}

int lowerServo1(){
 
  servo1.attach(servo1Pin);
   
  for (int i = servo1Pos; i >= SERVO1_DOWN_POS; i -= 1) {
      servo1.write(i);
      delay(SERVO_PULSE_DELAY);
    }   
  delay(SERVO_SETTLE_DELAY);
  servo1Pos = SERVO1_DOWN_POS;
  
  servo1.detach();
  return 0;
}

int lowerServo2(){

  servo2.attach(servo2Pin);  

  for (int i = servo2Pos; i <= SERVO2_DOWN_POS; i += 1) {
      servo2.write(i);
      delay(SERVO_PULSE_DELAY);
    }   

  delay(SERVO_SETTLE_DELAY);
  servo2Pos = SERVO2_DOWN_POS;

  servo2.detach();
  return 0;
}



int displayPellet(whichServo side) {
  
  if(side == left){
    
    servo1.attach(servo1Pin);

    // Lower arm to grab pellet.
    for (int i = servo1Pos; i >= SERVO1_DOWN_POS; i -= 1) {
      servo1.write(i);
      delay(SERVO_PULSE_DELAY);
    }   
    
    // Raise arm to display pellet
    for (int i = SERVO1_DOWN_POS; i <= SERVO1_UP_POS; i += 1) {
      servo1.write(i);
      delay(SERVO_PULSE_DELAY);
    }
    delay(SERVO_SETTLE_DELAY);
    servo1Pos = SERVO1_UP_POS;
    servo1_up_flag = true;
    servo1.detach();
  }
  else if(side == right){
     
    servo2.attach(servo2Pin);
    // Lower arm to grab pellet.
    for (int i = servo2Pos; i <= SERVO2_DOWN_POS; i += 1) {
      servo2.write(i);
      delay(SERVO_PULSE_DELAY);
    }   
    // Raise arm to display pellet
    for (int i = SERVO2_DOWN_POS; i >= SERVO2_UP_POS; i -= 1) {
      servo2.write(i);
      delay(SERVO_PULSE_DELAY);
    }
    delay(SERVO_SETTLE_DELAY);
    servo2Pos = SERVO2_UP_POS;
    servo2_up_flag = true;
    servo2.detach();
  }

  return 0;
}



int zeroStepper() {

  digitalWrite(A3, LOW);
  for(int i = 0; i < 1000; i++)
  {
    digitalWrite(A4, HIGH);
    delay(1);
    digitalWrite(A4, LOW);
    delay(1);
  }


  digitalWrite(A3, HIGH);
  delay(100);
  while(!switchState){

    digitalWrite(A4, HIGH);
    delay(1);
    digitalWrite(A4, LOW);
    delay(1);
  }

  for(int i = 0; i < 200; i++)
  {
    digitalWrite(A4, HIGH);
    delay(1);
    digitalWrite(A4, LOW);
    delay(1);
  }

  stepperDistFromOrigin = 0;
  return 0;
}

int moveStepper(int targetPos) {

  int travelDist = targetPos - stepperDistFromOrigin;
  
  if(travelDist < 0){

    // If we try to move backwards but switch is pressed, set current position to origin and block movement.
    if(switchState){
      stepperDistFromOrigin = 0;
      return 2;
    }
    digitalWrite(A3, HIGH);
  }
  else{

    digitalWrite(A3, LOW);
  }
  
  int stepsToTake = abs(travelDist);
  int stepsTaken = 0;
  
  for(int i = 0; i < stepsToTake; i += 1){

    digitalWrite(A4, HIGH);
    delay(1);
    digitalWrite(A4, LOW);
    delay(1);

    if(travelDist < 0){
      
      stepperDistFromOrigin -= 1;    
    }
    else if(travelDist > 0){
      
      stepperDistFromOrigin += 1;
    }  
    stepsTaken += 1;

    // If we hit the limit switch after a short movement, we're at the origin and should abort. 
    if(stepsTaken > 1000 && switchState){

      stepperDistFromOrigin = 0;
      return 1;
    }
   
  }
  return 0;
}





void setup() {

  // Open serial connection
  // Note: This takes a bit to connect so while(!Serial) keeps it waiting
  // until the connection is ready.
  Serial.begin(9600);
  while (!Serial) {
    delay(100);
  }

  zeroServos();



  // Set switch read pin
  pinMode(switchPin, INPUT_PULLUP);
  switchState = digitalRead(switchPin);
  attachInterrupt(digitalPinToInterrupt(switchPin), handleSwitchChange, CHANGE);
  zeroStepper();

  
  // Set IR breaker read pin
  pinMode(IRBreakerPin, INPUT_PULLUP);
  IRState = digitalRead(IRBreakerPin);
  attachInterrupt(digitalPinToInterrupt(IRBreakerPin), handleIRChange, HIGH);
  // Set LED control pin
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH);
  pinMode(ptgreyGPIOSignalPin, OUTPUT);
  digitalWrite(ptgreyGPIOSignalPin, HIGH);
  // Let client know we're ready
  Serial.write("READY\n");
}


bool listenForStartCommand() {
  
  char authByte;
  
  while(true) {

    if(Serial.available() > 0) {
      
      authByte = Serial.read();
    }
    
    if(authByte == 'A') {
      SIMULATION_MODE = false;
      return true;
    }
    else if (authByte == 'Y' ) {
      SIMULATION_MODE = false;
      digitalWrite(ledPin,HIGH);
      return false;
    }
    else if (authByte == 'S') {
      SIMULATION_MODE = true;
      return true;
    }
    else if (authByte == 'Z') {
      SIMULATION_MODE = true;
      return false;
    }

  }
}



int startSession() {

  while(!digitalRead(IRBreakerPin)) {

    char cmd;
    char stepperDist;
    
    if(Serial.available() > 0) {

      cmd = Serial.read();
      
      switch(cmd){
        
        case ('1'):
          displayPellet(right);
          break;
          
        case ('2'):
          displayPellet(left);
          break;
          
        case ('3'):

          // Give client a second to respond
          delay(1000);
          stepperDist = Serial.read();
          
          switch(stepperDist){
            case('0'):
              zeroStepper();
              break;
            case('1'):
              moveStepper(stepsToMmRatio * 1);
              break;
            case('2'):
              moveStepper(stepsToMmRatio * 2);
              break;
            case('3'):
              moveStepper(stepsToMmRatio * 3);
              break;
            case('4'):
              moveStepper(stepsToMmRatio * 4);
              break;
            case('5'):
              moveStepper(stepsToMmRatio * 5);
              break;
            case('6'):
              moveStepper(stepsToMmRatio * 6);
              break;
            default:
              break;  
            }
            
        default:
          break;
      }
    }
    
  }


  char termCmd;
  // Send session end message.
  Serial.write("TERM\n");

  if(servo1_up_flag)
  {
    lowerServo1();
    servo1_up_flag = false;
  }
  if(servo2_up_flag)
  {
    lowerServo2();
    servo2_up_flag = false;
  }
  // Flush serial buffer.
  while(Serial.read() >= 0) {
    continue;
  }

  return 0;
}


int startSimulatedSession() {
  
  long StartTime = millis();
  long CurrentTime = StartTime;
  long randomDuration = random(1,20000);
  
  while((CurrentTime - StartTime) < randomDuration) {

    CurrentTime = millis();

    char cmd;
    char stepperDist;
    
    if(Serial.available() > 0) {

      cmd = Serial.read();
      
      switch(cmd){
        
        case ('1'):
          displayPellet(right);
          break;
          
        case ('2'):
          displayPellet(left);
          break;
          
        case ('3'):

          // Give client a second to respond
          delay(1000);
          stepperDist = Serial.read();
          
          switch(stepperDist){
            case('0'):
              zeroStepper();
              break;
            case('1'):
              moveStepper(stepsToMmRatio * 1);
              break;
            case('2'):
              moveStepper(stepsToMmRatio * 2);
              break;
            case('3'):
              moveStepper(stepsToMmRatio * 3);
              break;
            case('4'):
              moveStepper(stepsToMmRatio * 4);
              break;
            case('5'):
              moveStepper(stepsToMmRatio * 5);
              break;
            case('6'):
              moveStepper(stepsToMmRatio * 6);
              break;
            default:
              break;  
            }
            
        default:
          break;
      }
    }
    
  }


  char termCmd;
  // Send session end message.
  Serial.write("TERM\n");

  if(servo1_up_flag)
  {
    lowerServo1();
    servo1_up_flag = false;
  }
  if(servo2_up_flag)
  {
    lowerServo2();
    servo2_up_flag = false;
  }
  // Flush serial buffer.
  while(Serial.read() >= 0) {
    continue;
  }

  return 0;  
}


void loop() { 

  if(listenForStartCommand()){

      if(SIMULATION_MODE) {
        startSimulatedSession();
      }
      else {
        startSession();
      }
  }       
}

 
