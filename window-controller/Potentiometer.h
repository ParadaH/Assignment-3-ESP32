#ifndef __POTENTIOMETER_H__
#define __POTENTIOMETER_H__

#include <Arduino.h>

class Potentiometer {
public:
    Potentiometer(int pin);
    int read();

private:
    int pin;
};

#endif
