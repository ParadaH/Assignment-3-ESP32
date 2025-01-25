#ifndef __LED__
#define __LED__

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