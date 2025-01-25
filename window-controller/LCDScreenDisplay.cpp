#include "LCDScreenDisplay.h"
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); //Hubert's board

void LCDScreenDisplay::init() {
    lcd.init();
    lcd.backlight();
}

void LCDScreenDisplay::updateMsg(const String& message) {
    String line1, line2;
    splitMessage(message, line1, line2);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(line1);
    lcd.setCursor(0, 1);
    lcd.print(line2);
}

void LCDScreenDisplay::splitMessage(const String& input, String& line1, String& line2) {
    if (input.length() <= 16) {
        line1 = input;
        line2 = "";
    } else {
        int splitIndex = input.lastIndexOf(' ', 16);

        if (splitIndex != -1) {
            line1 = input.substring(0, splitIndex);
            line2 = input.substring(splitIndex + 1);
        } else {
            line1 = input.substring(0, 16);
            line2 = input.substring(16);
        }
    }
}

void LCDScreenDisplay::turnOff() {
    lcd.noDisplay();
    lcd.noBacklight();
}

void LCDScreenDisplay::turnOn() {
    lcd.display();
    lcd.backlight();
}

void LCDScreenDisplay::updateWindowParameters(float temperature, bool isManualMode, int windowPosition){
    String message = "Temp: " + String(temperature) + " C   ";
    
    if(isManualMode){
        message += "Manual: ";
    } 
    else if(!isManualMode){
        message += "Auto: ";
    }
    message += String(windowPosition) + "%";
    
    updateMsg(message);
}