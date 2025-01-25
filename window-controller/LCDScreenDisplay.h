#ifndef __LCD_SCREEN_DISPLAY_H__
#define __LCD_SCREEN_DISPLAY_H__
#include "Arduino.h"
#include <LiquidCrystal_I2C.h>

class LCDScreenDisplay {
public:
    void init();
    void updateMsg(const String& message);
    void turnOff();
    void turnOn();
    void updateWindowParameters(float temperature, bool isManualMode, int windowPosition);

private:
    void splitMessage(const String& input, String& line1, String& line2);
};

#endif