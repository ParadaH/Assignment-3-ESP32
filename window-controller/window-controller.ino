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

String initMsg = "Starting...";

int currentState = IDLE;
bool isManualMode = false;
int windowPosition = 0;
float currentTemperature = 22;
float tempThreshold = 35;

long lastLCDUpdateTime = 0;
int LCDUpdateFrequency = 500;

Button button(BUTTON_PIN, &isManualMode);
Servomotor windowServo;
Potentiometer potentiometer(POTENTIOMETER_PIN);
LCDScreenDisplay screenDisplay;

void handleButton(){
  static bool lastButtonState = LOW;
  bool buttonState = digitalRead(BUTTON_PIN);

  if (buttonState == HIGH && lastButtonState == LOW) {
    isManualMode = !isManualMode;
  }
  lastButtonState = buttonState;
}

void updateWindowPosition(){
  if(currentTemperature < 20){
    windowPosition = 0;
  }
  else if(currentTemperature >= 20 && currentTemperature < 25){
    windowPosition = map(currentTemperature, 20, 25, 0, 50);
  }
  else if(currentTemperature >= 25){
    windowPosition = 100;
  }

  int servoAngle = map(windowPosition, 0, 100, 0, 90);
  windowServo.write(servoAngle);
}

void setup(){
  Serial.begin(9600);
  windowServo.attach(SERVO_PIN);
  windowServo.write(0);

  pinMode(BUTTON_PIN, INPUT);

  screenDisplay.init();
  screenDisplay.updateMsg(initMsg);
  delay(2000);
  screenDisplay.updateMsg("");
}

void loop(){

  switch (currentState) {
    case IDLE:
    {
      if(isManualMode){
        currentState = MANUAL_MODE;
      } 
      else if(!isManualMode){
        currentState = AUTO_MODE;
      }
      break;
    }

    case MANUAL_MODE:
    {
        windowPosition = potentiometer.read();
        int servoAngle = map(windowPosition, 0, 100, 0, 90); // 0-100% -> 0-90 deg
        windowServo.write(servoAngle);
        currentState = IDLE;
        break;
    }
      
    case AUTO_MODE:
    {
      updateWindowPosition();
      if(currentTemperature > tempThreshold){
        currentState = ALARM;
        break;
      }
      currentState = IDLE;
      break;
    }
      
    case ALARM:
    {
      windowServo.write(0);
      if (currentTemperature <= tempThreshold) {
        currentState = IDLE;
      }
      break;
    }
  }

  if(millis() - lastLCDUpdateTime >= LCDUpdateFrequency){
    lastLCDUpdateTime = millis();
    if(currentState == ALARM){
      screenDisplay.updateMsg("ALARM!       Temp too high!");
    }
    if(currentState != ALARM){
      screenDisplay.updateWindowParameters(currentTemperature, isManualMode, windowPosition);
    }
  }

  button.handleButton();
}