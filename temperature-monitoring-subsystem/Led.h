#ifndef __LED_H__
#define __LED_H__

#include <Arduino.h>

class Led {
private:
    int greenPin;
    int redPin;

public:
    Led(int greenPin, int redPin);
    void init();
    void setGreen();
    void setRed();
    void turnOff();
};

#endif