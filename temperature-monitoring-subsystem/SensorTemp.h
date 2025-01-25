#ifndef __SENSORTEMP_H__
#define __SENSORTEMP_H__

#include <Arduino.h>

class TemperatureSensor {
private:
    int pin;
    int offset;

public:
    TemperatureSensor(int pin, int offset);
    float readTemperature();
};

#endif