#include "Button.h"
#include "LCDScreenDisplay.h"
#include "Potentiometer.h"
#include "Servomotor.h"

#define SERVO_PIN 9
#define BUTTON_PIN 7
#define POTENTIOMETER_PIN A0

#define IDLE 101
#define MANUAL_MODE 102
#define AUTO_MODE 103
#define ALARM 104
#define CHECKTEMP 105

#define T1 20
#define T2 30

String initMsg = "Starting...";
String alarmMsg = "ALARM!       Temp too high!";

int currentState = IDLE;
bool isManualMode = false;
int windowPosition = 0;
int servoAngle = 0;
float T = 23;

long lastLCDUpdateTime = 0;
int LCDUpdateFrequency = 500;

bool resetAlarmCommand = false;
bool alarmCommand = false;

Button button(BUTTON_PIN, &isManualMode);
Servomotor windowServo;
Potentiometer potentiometer(POTENTIOMETER_PIN);
LCDScreenDisplay screenDisplay;

void readFromSerial(){
  if(Serial.available() > 0){

    String receivedData = Serial.readStringUntil("\n");
    
    Serial.println("Information acknowledged: " + String(receivedData));

    if(receivedData == "RESET_ALARM"){
      if(currentState == ALARM){
        resetAlarmCommand = true;
      }
      else {
        resetAlarmCommand = true;
        /* callback that there is no alarm */
      }
    }
    else if(receivedData == "ALARM"){
      alarmCommand = true;
    }
    else {
      T = receivedData.toFloat();
    }
  }
}

void invalidate(){
  if(millis() - lastLCDUpdateTime >= LCDUpdateFrequency){
    lastLCDUpdateTime = millis();

    if(alarmCommand == true){
      screenDisplay.updateMsg(alarmMsg);
    }
    else if(alarmCommand == false){
      screenDisplay.updateWindowParameters(T, isManualMode, windowPosition);
    }
  }
}

void updateWindowPosition(){
  if(T < T1){
    windowPosition = 0;
  }
  else if(T >= T1 && T < T2){
    windowPosition = map(T, 20, 30, 0, 90);
  }
  else if(T >= T2){
    windowPosition = 100;
  }

  servoAngle = map(windowPosition, 0, 100, 0, 90);
  windowServo.write(servoAngle);
}

void setup(){
  Serial.begin(9600);
  windowServo.attach(SERVO_PIN);
  windowServo.write(0);

  pinMode(BUTTON_PIN, INPUT);

  screenDisplay.init();
  screenDisplay.updateMsg(initMsg);
  delay(1000);
  screenDisplay.updateMsg("");
}

void loop(){

  button.handleButton();
  readFromSerial();

  switch (currentState) {
    case IDLE: {
      if(alarmCommand == false){
        if(isManualMode){
          currentState = MANUAL_MODE;
        } 
        else if(!isManualMode){
          if(alarmCommand = false){
            currentState = ALARM;
            break;
          }
          currentState = AUTO_MODE;
        }
      }
      else {
        currentState = ALARM;
      }
        
      break;
    }

    case MANUAL_MODE: {
        windowPosition = potentiometer.read();
        int servoAngle = map(windowPosition, 0, 100, 0, 90); // 0-100% -> 0-90 deg
        windowServo.write(servoAngle);
        currentState = IDLE;
        break;
    }
      
    case AUTO_MODE: {
      updateWindowPosition();
      currentState = IDLE;
      break;
    }
      
    case ALARM: {
      if(resetAlarmCommand){
        currentState = IDLE;
        resetAlarmCommand = false;
        alarmCommand = false;
      }
      windowServo.write(0);
      break;
    }
  }
  invalidate();
}