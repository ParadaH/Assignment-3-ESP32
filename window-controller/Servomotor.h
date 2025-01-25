#ifndef __SERVOMOTOR_H__
#define __SERVOMOTOR_H__

#include <Arduino.h>
#include <Servo.h>

class Servomotor {
private:
    Servo motor;
    int pin;

public:
    Servomotor();
    void attach(int pin);
    void write(int angle);
    void detach();
};

#endif
